/* Multi-core Task Handler*/
TaskHandle_t primaryTask;
TaskHandle_t secondaryTask;
TaskHandle_t animationTask;
SemaphoreHandle_t animationMutex;

/* Semaphore for handling mutex */
void createSemaphore() {
  animationMutex = xSemaphoreCreateMutex();
  xSemaphoreGive(animationMutex);
}

void mutexLock() {
  xSemaphoreTake(animationMutex, portMAX_DELAY);
}

void mutexUnlock() {
  xSemaphoreGive(animationMutex);
}

//Set the animation code, suppose to be used in core 0 only
void setAnimationCode(char filename) {
  Serial.println("Updating animation frame to " + String(filename));
  mutexLock();
  animation = filename;
  mutexUnlock();
}

//Get the animation code, suppose to be used in core 1 only
char getAnimationCode() {
  char anicode = 'a';
  mutexLock();
  anicode = animation;
  mutexUnlock();
  return anicode;
}

//Get the current state of the switch
// true = After human pushed
// false = After robot pushed
bool getSwitchState() {
  int switchState = digitalRead(TOGGLE_SWITCH);
  return (switchState == 1);
}

//Set the WiFi interface mode of the ESP32 based on start flags or debug settings
//Return true if developer mode, return false for default mode (auto mode)
bool initPrimaryLogicNetworkMode() {
  clearFrame();
  bool switchPushed = getSwitchState();
  if (switchPushed || ENABLE_WIFI_DEBUG) {
    /* Switch was on when device power on. Start WiFi & Web Server */
    //Start AP and web server
    if (ENABLE_WIFI_DEBUG) {
      setAnimationCode('x');  //WiFi Error
      //Use WiFi client mode
      WiFi.mode(WIFI_STA);  //Optional
      WiFi.begin(DEBUG_SSID, DEBUG_PWD);
      Serial.println("\nConnecting");
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
      }
      Serial.println("\nConnected to the WiFi network");
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
      setAnimationCode('w');  //WiFi Connected
    } else {
      //Set display to AP icon
      setAnimationCode('w');
      WiFi.softAP(AP_SSID, NULL);
      Serial.print("Manual mode started. SSID=" + String(AP_SSID) + " listening on : ");
      Serial.println(WiFi.softAPIP());
      //Setup DNS Server
      dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
      dnsServer.start(53, "*", WiFi.softAPIP());
    }

    // Start the web server
    registerAPIEndpoints();
    server.begin();
    return true;
  }
  return false;
}

/* Multi-core process definations */
void startCoreTasks() {
  //Pin animation rendering routine to core 0
  xTaskCreatePinnedToCore(
    AnimationController, /* Task function. */
    "animator",          /* name of task. */
    8192,                /* Stack size of task */
    NULL,                /* parameter of the task */
    2,                   /* priority of the task */
    &animationTask,
    0);

  //Pin the primary task routine to core 1
  xTaskCreatePinnedToCore(
    PrimaryController, /* Task function. */
    "primary",         /* name of task. */
    24576,             /* Stack size of task */
    NULL,              /* parameter of the task */
    1,                 /* priority of the task */
    &primaryTask,      /* Task handle to keep track of created task */
    1);

  //Pin secondary tasks to core 1 for handling AP movements
  xTaskCreate(
    SecondaryController,
    "secondary",
    2048,
    NULL, 
    3,
    &secondaryTask
  );
  
  vTaskStartScheduler();
}

//For movement in development mode
void SecondaryController(void* pvParameters) {
  Serial.println("Secondary logic process started on core " + String(xPortGetCoreID()));
  while (1) {
    if (movingActive) {
      if (movingDirection == 0) {
        forward(1);
      } else if (movingDirection == 1) {
        backward(1);
      } else if (movingDirection == 2) {
        rotateAntiClockwise(1);
      } else if (movingDirection == 3) {
        rotateClockwise(1);
      } else {
        // Unknown direction
        delay(100);
        return;
      }
    } else {
      // Delay for a while before checking again
      delay(100);
    }
  }
}

//For movement and primary logics
void PrimaryController(void* pvParameters) {
  Serial.println("Primary logic process started on core " + String(xPortGetCoreID()));
  if (!initPrimaryLogicNetworkMode()) {
    /* Switch is off during power on. Use automatic mode */
    int seqCounter = 0;  //Modify this value to change start state of seq
    bool switchPushed = false;
    while (1) {
      switchPushed = getSwitchState();
      if (switchPushed) {
        //Switch pushed
        executePushAnimationSequence(seqCounter);
        seqCounter++;
      }
      delay(1);
    }
  } else {
    while (1) {
      dnsServer.processNextRequest();
      delay(1);
    }
  }
}

//For animation rendering
void AnimationController(void* pvParameters) {
  Serial.println("Animation render started on core " + String(xPortGetCoreID()));
  while (1) {
    char anicode = getAnimationCode();
    handleAnimationRendering(anicode);
    delay(1);
  }
}
