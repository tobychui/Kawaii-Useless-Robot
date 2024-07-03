/*
  Main logic handlers

  This file store the behavior of the robot
  when the switch is pressed

*/

/* Pushing Sequences */
void executePushAnimationSequence(int seqID) {
  if (seqID < 3) {
    //Default push back
    setAnimationCode('a');
    delay(4000);
    pushSwitchDelayed(1000, 1000);
    if (seqID == 2) {
      //Do not turn off display
      return;
    }
  } else if (seqID < 4) {
    //Hesitation push back
    setAnimationCode('a');
    delay(4000);
    pushWithHesitation();
    return;
  } else if (seqID < 5) {
    //Hesitation push and walk back
    setAnimationCode('a');
    delay(4000);
    servoCoverPusher.write(90);
    delay(1000);
    servoSwitchPusher.write(90 + SERVO_ALIGNMENT_OFFSET);
    delay(2000);
    backward(100);
    delay(1000);
    servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
    delay(1000);
    servoCoverPusher.write(0);
    servoSwitchPusher.write(0);
    return;
  } else if (seqID < 8) {
    //Annoy push back
    setAnimationCode('b');
    delay(2000);
    pushSwitchDelayed(300, 300);
    delay(3000);
    //Do not return to clear-frames when annoyed
    return;
  } else if (seqID < 10) {
    //Faster annoyed push back
    setAnimationCode('b');
    delay(500);
    pushSwitchNow();
    delay(1000);
    //Do not return to clear-frames when annoyed
    return;
  } else if (seqID < 11) {
    //Shake its head and push back
    setAnimationCode('f');
    delay(500);
    rotateClockwise(50);
    rotateAntiClockwise(100);
    rotateClockwise(100);
    rotateAntiClockwise(50);
    pushSwitchDelayed(1000, 300);
    delay(2000);
    setAnimationCode('g');
    return;
  } else if (seqID < 13) {
    //Annoyed fast push back
    setAnimationCode('g');
    delay(500);
    pushSwitchNow();
    delay(1000);
    return;
  } else if (seqID < 14) {
    //Angry & Movement
    setAnimationCode('y');
    delay(5000);
    setAnimationCode('c');
    delay(2000);
    servoCoverPusher.write(90);
    delay(1000);
    forward(100);
    backward(100);
    forward(100);
    backward(100);
    pushSwitchNow();
    return;
  } else if (seqID < 17) {
    //Angry fast push
    delay(500);
    if (seqID % 2 == 0) {
      forward(100);
    } else {
      backward(100);
    }
    pushSwitchNow();
    delay(1000);
    return;
  } else if (seqID < 18) {
    setAnimationCode('h');
    delay(1000);
    setAnimationCode('i');
    delay(1000);
    setAnimationCode('j');
    forward(50);
    backward(50);
    forward(50);
    rotateClockwise(50);
    rotateAntiClockwise(100);
    rotateClockwise(100);
    rotateAntiClockwise(50);
    pushSwitchDelayed(1000, 300);
    return;
  } else if (seqID < 22) {
    delay(500);
    pushSwitchDelayed(1000, 300);
    return;
  } else if (seqID < 25) {
    setAnimationCode('d');
    delay(500);
    pushWithHesitation();
    return;
  } else {
    //fallback default push back
    delay(500);
    int expression = random(0, 4);
    if (expression == 0) {
      setAnimationCode('a');
      delay(3000);
      pushSwitchDelayed(1000, 1000);
      delay(500);
    } else if (expression == 1) {
      setAnimationCode('b');
      delay(500);
      pushSwitchNow();
      delay(500);
    } else if (expression == 2) {
      setAnimationCode('g');
      pushWithHesitation();
      delay(500);
    } else {
      setAnimationCode('e');
      pushSwitchNow();
      return;
    }
  }
  clearFrame();
  delay(500);
}

//Debug sequence to test all movement functions
void runDebugSequence() {
  //setAnimationCode('a');
  //delay(10000);

  //Stepper test
  forward(50);
  delay(3000);
  backward(50);
  delay(3000);
  rotateAntiClockwise(100);
  delay(1000);
  //rotate backward
  rotateClockwise(100);
  delay(3000);

  //setAnimationCode('j');
  //delay(10000);
  //Servo test
  servoCoverPusher.write(90 + SERVO_ALIGNMENT_OFFSET);
  delay(1000);
  servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
  delay(1000);
  servoCoverPusher.write(0);
  servoSwitchPusher.write(0);
  delay(3000);

  //Switch test
  bool switchPushed = getSwitchState();
  if (switchPushed) {
    Serial.println("Switch pushed");
  } else {
    Serial.println("Switch idle");
  }
}
