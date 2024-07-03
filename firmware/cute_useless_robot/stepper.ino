/* Stepper Drivers Functions */

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

//Move the robot forward by 8 * rev micro steps
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
  standbySteppers();
}

//Move the robot backward by 8 * rev micro steps
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
  standbySteppers();
}

//Differential offset the robot wheels by +- 8 * rev micro steps, anti-clockwise
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
  standbySteppers();
}

//Differential offset the robot wheels by +- 8 * rev micro steps, clockwise
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
  standbySteppers();
}
