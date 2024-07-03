#include <Servo.h>

Servo servo1;
Servo servo2;

// Pin definitions
#define DATA_PIN 4  // DS
#define CLOCK_PIN 16  // SH_CP
#define LATCH_PIN 17  // ST_CP

void setup() {
  // Initialize pins
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  //Initialize servos
  servo1.attach(14);
  servo2.attach(27);
}

void writeStep(byte stepByte) {
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, stepByte);
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(LATCH_PIN, LOW);
  delayMicroseconds(840); //780 min
}

void forward(int rev) {
  for (int i = 0; i < rev; i++) {
    writeStep(0b00010001);
    writeStep(0b00110011);
    writeStep(0b00100010);
    writeStep(0b01100110);
    writeStep(0b01000100);
    writeStep(0b11001100);
    writeStep(0b10001000);
    writeStep(0b10011001);
  }

}

void backward(int rev) {
  for (int i = 0; i < rev; i++) {
    writeStep(0b10011001);
    writeStep(0b10001000);
    writeStep(0b11001100);
    writeStep(0b01000100);
    writeStep(0b01100110);
    writeStep(0b00100010);
    writeStep(0b00110011);
    writeStep(0b00010001);
  }
}

void rotateAntiClockwise(int rev) {
  for (int i = 0; i < rev; i++) {
    writeStep(0b00011001);
    writeStep(0b00111000);
    writeStep(0b00101100);
    writeStep(0b01100100);
    writeStep(0b01000110);
    writeStep(0b11000010);
    writeStep(0b10000011);
    writeStep(0b10010001);
  }
}

void rotateClockwise(int rev) {
  for (int i = 0; i < rev; i++) {
    writeStep(0b10010001);
    writeStep(0b10000011);
    writeStep(0b11000010);
    writeStep(0b01000110);
    writeStep(0b01100100);
    writeStep(0b00101100);
    writeStep(0b00111000);
    writeStep(0b00011001);
  }
}

void loop() {
  //Stepper test
  forward(50);
  delay(3000);
  backward(50);
  delay(3000);
  //Servo test
  servo1.write(90);
  delay(1000);
  servo2.write(180);
  delay(1000);
  servo1.write(0);
  servo2.write(0);
  delay(3000);
  //rotateClockwise(100);
  //delay(3000);
  //rotateAntiClockwise(100);
  //delay(3000);
}
