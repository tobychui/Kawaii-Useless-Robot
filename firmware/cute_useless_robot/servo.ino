/*

  Servo Logic Code

  This script handles logic related to servo operations

*/

//Quickily push the switch back
void pushSwitchNow() {
  pushSwitchDelayed(270, 270);
}

//Push switch back with delay
void pushSwitchDelayed(int coverDelay, int pusherDelay) {
  servoCoverPusher.write(90);
  delay(coverDelay);
  servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
  delay(pusherDelay);
  handlePushBackException(); //Check if switch is properly pushed back
  servoCoverPusher.write(0);
  servoSwitchPusher.write(0);
}

//Push with a pause before the switch is pushed
void pushWithHesitation() {
  servoCoverPusher.write(90);
  delay(1000);
  servoSwitchPusher.write(90 + SERVO_ALIGNMENT_OFFSET);
  delay(2000);
  servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
  delay(1000);
  handlePushBackException(); //Check if switch is properly pushed back
  servoCoverPusher.write(0);
  servoSwitchPusher.write(0);
}

//Handle case where user hold the switch
void handlePushBackException() {
  bool switchPushed = getSwitchState();
  int triedResetCount = 0;
  if (switchPushed) {
    //Switch position still not reset. Wait and try again
    while (triedResetCount < 5) {
      switchPushed = getSwitchState();
      if (!switchPushed) {
        //Switch reset
        return;
      }
      if (triedResetCount < 1) {
        //Light push retry
        servoSwitchPusher.write(90 + SERVO_ALIGNMENT_OFFSET);
        delay(300);
        servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
        delay(1000);
      }else if (triedResetCount < 3) {
        //Light push retry
        servoSwitchPusher.write(90 + SERVO_ALIGNMENT_OFFSET);
        delay(500);
        servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
        delay(2000);
      } else {
        //Hard push retry
        servoSwitchPusher.write(0);
        delay(500);
        servoSwitchPusher.write(130 + SERVO_ALIGNMENT_OFFSET);
        delay(2000);
      }
      triedResetCount++;
    }

    //Max retry reached
    switchPushed = getSwitchState();
    if (switchPushed){
      char originalAnicode = animation;
      setAnimationCode('v');
      delay(5000);
      setAnimationCode(originalAnicode); //Restore the animation charcode
    }
   
    return;
  }
}

//Set the pusher servo to arm installation position
void setSwitchToInstallPosition() {
  servoSwitchPusher.write(125 + SERVO_ALIGNMENT_OFFSET);
}
