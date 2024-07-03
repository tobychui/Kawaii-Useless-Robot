/*

    Cute Useless Robot (´・ω・`)

    Developed by tobychui
    Idea come from Kairoshi

    Board Settings:
    ESP32 v2.014
    -> ESP32 Dev Module
    -> CPU Freq: 240Mhz
    -> Arduino & Events run on: Core 1
    -> Flash Mode: DIO
*/

/* Libraries */
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <ESP32Servo.h>  //Require ESP32Servo
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

/* Pins Definations */
#define STP_DATA_PIN 4    // Stepper Shift Register DS
#define STP_CLOCK_PIN 16  // Stepper Shift Register SH_CP
#define STP_LATCH_PIN 17  // Stepper Shift Register ST_CP

#define SERVO_SWITCH 27  //Servo to push the switch
#define SERVO_COVER 14   //Servo to push the cover

#define DP_CLK_PIN 32   //Display CLK
#define DP_DATA_PIN 33  //Display DIN
#define DP_CS_PIN 25    //Display CS

#define SD_CS_PIN 5       //SD Card CS pin
#define TOGGLE_SWITCH 13  //Switch on top of the matrix display

/* Display settings generated by trial and error. Don't touch these */
#define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW
#define MAX_DEVICES 8

/* WiFI AP Settings */
#define AP_SSID "(´・ω・`)"
#define ENABLE_WIFI_DEBUG false //Set to true to use WiFi Client mode for remote debugging
#define DEBUG_SSID "" //Debug SSID, usually your home WiFi SSID
#define DEBUG_PWD "" //Debug Password, your home WiFi Password
AsyncWebServer server(80);
DNSServer dnsServer;

/* Calibrated offset for switch pusher servo, in degrees */
#define SERVO_ALIGNMENT_OFFSET 4

/* Hardware Type Definations */
Servo servoSwitchPusher;
Servo servoCoverPusher;
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DP_DATA_PIN, DP_CLK_PIN, DP_CS_PIN, MAX_DEVICES);

/* Global Variables */
char animation = 'a';  //Animation ID to render
int movingDirection = 0; //Debug mode -> Moving direction
bool movingActive = false; //Debug mode -> Is Moving direction active

void setup() {
  delay(1000);
  Serial.begin(115200);

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  /* Stepper IO */
  pinMode(STP_DATA_PIN, OUTPUT);
  pinMode(STP_CLOCK_PIN, OUTPUT);
  pinMode(STP_LATCH_PIN, OUTPUT);
  standbySteppers();

  /* Servo IO */
  servoSwitchPusher.setPeriodHertz(50);
  servoCoverPusher.setPeriodHertz(50);
  servoSwitchPusher.attach(SERVO_SWITCH);
  servoCoverPusher.attach(SERVO_COVER);
  servoCoverPusher.write(0);
  servoSwitchPusher.write(0);

  /* Display Module */
  mx.begin();
  setDisplayBrightness(0x4);
  renderFrame();  //Render the default frame to matrix

  /* SD Card */
  // Initialize SD card
  if (!SD.begin(SD_CS_PIN, SPI, 4000000U, "/sd", 10U, false)) {
    Serial.println("[Error] Unable to mount SD card. Retrying in 10 seconds");
    loadSDErrorToFrameBuffer();  //Render SD ERROR to display
    renderFrame();

    //Retry in 10 seconds
    delay(5000);
    ESP.restart();
  }

  /* Start Dual-core processes */
  createSemaphore();
  startCoreTasks();
}

void loop() {
  //Process has been handled in tasks.ino
  //Do not use this loop
}
