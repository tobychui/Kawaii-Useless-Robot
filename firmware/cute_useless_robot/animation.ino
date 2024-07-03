/* Animation Rendering Logics */

int frameCount = 0;
char previousAnicode = 'a';
const String animationFolder = "/anime/"; //Need tailing slash

//delay with early breakout if current animation code changed
void delayWithEarlyBreakout(int delayDuration) {
  delayDuration = delayDuration / 100;
  for (int i = 0; i < delayDuration; i++) {
    vTaskDelay(pdMS_TO_TICKS(100));
    if (getAnimationCode() != previousAnicode) {
      break;
    }
  }
}

//Handle animation rendering
void handleAnimationRendering(char anicode) {
  if (previousAnicode != anicode) {
    previousAnicode = anicode;
    frameCount = 0;
  }
  //Check if the target animation frame is static
  String targetFrame = animationFolder + String(anicode) + ".bin";
  if (SD.exists(targetFrame)) {
    //This is a static frame. Load and render it
    loadFrameAndRender(targetFrame);
    int delayDuration = getFrameDuration(anicode, 0);
    delayWithEarlyBreakout(delayDuration);
    return;
  }

  //It is not a static frame. Check if x0.bin exists
  targetFrame = animationFolder + String(anicode) + String(frameCount) + ".bin";
  if (SD.exists(targetFrame)) {
    loadFrameAndRender(targetFrame);
    frameCount++;
    int delayDuration = getFrameDuration(anicode, frameCount);
    delayWithEarlyBreakout(delayDuration);
    return;
  } else {
    //Not found.
    if (frameCount != 0) {
      loadFrameAndRender(animationFolder + String(anicode) + "0.bin");
      int delayDuration = getFrameDuration(anicode, 0);
      delayWithEarlyBreakout(delayDuration);
      frameCount = 1;
      return;
    } else {
      //frameCount = 0. x0.bin not exists. Report error
      Serial.println("Target frame buffer not found: " + String(anicode) + ".bin");
      setAnimationCode('a');
      return;
    }
  }
}

//Get how long the frame shd last.
//Default 500 unless specially programmed
int getFrameDuration(char anicode, int framecount) {
  if ((anicode == 'a' || anicode == 'b' || anicode == 'g')) {
    //Blinking
    if (framecount == 0) {
      return 3000;
    }
    return 300;
  }else if (anicode == 'y'){
    //Transits, total duration = 3000ms
    if (framecount == 46) {
      return 2540;
    }
    return 1;
  }
  return 500;
}
