/*

   Servo Alignment Script

   This script set the servo to installation positions
   (as well as help debugging servo wires)


*/
#include <ESP32Servo.h> //Require ESP32Servo

#define STP_DATA_PIN 4    // Stepper Shift Register DS
#define STP_CLOCK_PIN 16  // Stepper Shift Register SH_CP
#define STP_LATCH_PIN 17  // Stepper Shift Register ST_CP

#define SERVO_SWITCH 27  //Servo to push the switch
#define SERVO_COVER 14   //Servo to push the cover

#define TOGGLE_SWITCH 13  //Switch on top of the matrix display
#define SERVO_ALIGNMENT_OFFSET 3

/* Hardware Type Definations */
Servo servoSwitchPusher;
Servo servoCoverPusher;

//Get the current state of the switch
// true = After human pushed
// false = After robot pushed
bool getSwitchState() {
  int switchState = digitalRead(TOGGLE_SWITCH);
  return (switchState == 1);
}

//Push switch back with delay
void pushSwitchDelayed(int coverDelay, int pusherDelay) {
  servoCoverPusher.write(90);
  delay(coverDelay);
  servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
  delay(pusherDelay);
  servoCoverPusher.write(0);
  servoSwitchPusher.write(0);
}

//writeStep write the step given the byte representation of a step on both stepper
//e.g. writeStep(0b00110011);
void writeStep(byte stepByte) {
  shiftOut(STP_DATA_PIN, STP_CLOCK_PIN, MSBFIRST, stepByte);
  digitalWrite(STP_LATCH_PIN, HIGH);
  digitalWrite(STP_LATCH_PIN, LOW);
  delayMicroseconds(840); //780 - 800 ms minimum, recommend 840 to 860ms
}

//Set all coils to off and enter standby mode
//to save power and prevent overheating
void standbySteppers() {
  writeStep(0b00000000);
}

void setup() {
  Serial.begin(115200);

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // put your setup code here, to run once:
  /* Servo IO */
  servoSwitchPusher.setPeriodHertz(50);
  servoCoverPusher.setPeriodHertz(50);
  servoSwitchPusher.attach(SERVO_SWITCH);
  servoCoverPusher.attach(SERVO_COVER);
  servoCoverPusher.write(0);
  servoSwitchPusher.write(0);
  //Set servo pusher to vertical install position
  delay(1000);
  servoSwitchPusher.write(125);

  /* Stepper IO */
  pinMode(STP_DATA_PIN, OUTPUT);
  pinMode(STP_CLOCK_PIN, OUTPUT);
  pinMode(STP_LATCH_PIN, OUTPUT);
  standbySteppers();
}


void loop() {
  // put your main code here, to run repeatedly:
  //Switch test
  bool switchPushed = getSwitchState();
  if (switchPushed) {
    servoSwitchPusher.write(0);
    Serial.println("Switch pushed");
    delay(1000);
    pushSwitchDelayed(300,300);
    delay(3000);
    servoSwitchPusher.write(125);
  } else {
    Serial.println("Switch idle");
  }
  delay(500);
}
