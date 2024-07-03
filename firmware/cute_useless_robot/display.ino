/* 
  LED Matrix Display Driver 
  
  Library doc
  https://majicdesigns.github.io/MD_MAX72XX/class_m_d___m_a_x72_x_x.html
*/

#define FRAME_BUFFER_SIZE 64  //4(32 bits) x 16 bytes

//SD error frame buffer. If animation read failed, this
//will be copied to frame buffer insteads
const unsigned char sd_error_fb[] = {
  0x00, 0x00, 0x00, 0x00,
  0x77, 0x3b, 0x9c, 0x00,
  0x44, 0xa2, 0x52, 0x00,
  0x74, 0xbb, 0x9c, 0x00,
  0x14, 0xa2, 0x52, 0x00,
  0x77, 0x3a, 0x52, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

//frame buffer, 32x16px
//each LED is 1 bit
unsigned char frame_buffer[] = {
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x22, 0x00, 0x00, 0x44,
  0x44, 0x00, 0x00, 0x22,
  0x89, 0x80, 0x01, 0x91,
  0x92, 0x40, 0x02, 0x49,
  0x82, 0x40, 0x02, 0x41,
  0x81, 0x80, 0x01, 0x81,
  0x80, 0x04, 0x40, 0x01,
  0x80, 0x09, 0x20, 0x01,
  0x87, 0x89, 0x21, 0xe1,
  0x40, 0x06, 0xc0, 0x02,
  0x27, 0x80, 0x01, 0xe4,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

//A combined function for read frame to framebuffer and render to display
//Example usage: loadFrameAndRender("/a.bin");
void loadFrameAndRender(String filepath) {
  readFrameToFrameBuffer(filepath);
  renderFrame();
}

//Load the SD read error message to framebuffer
void loadSDErrorToFrameBuffer(){
  //sd_error_fb and frame_buffer should be the same
  size_t bufferSize = sizeof(sd_error_fb) / sizeof(sd_error_fb[0]);
  for (size_t i = 0; i < bufferSize; i++) {
    frame_buffer[i] = sd_error_fb[i];
  }
}

//Read a binary file from SD card to current framebuffer
int readFrameToFrameBuffer(String filepath) {
  File file = SD.open(filepath, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return 1;
  }

  // Read file byte by byte
  size_t bytesRead = 0;
  while (file.available() && bytesRead < FRAME_BUFFER_SIZE) {
    frame_buffer[bytesRead] = file.read();
    bytesRead++;
  }

  // Close the file
  file.close();
  return 0;
}



/*
 * renderFrame render the frame buffer to display 
 * 
 * The display is an upside down two split LED grid matrix display
 * the render sequence (when viewed from front) is as follows 
 * and each matrix module is upside down (row 0 on bottom)
 * [8][7][6][5]
 * [4][3][2][1]
 * 
 */
void renderFrame() {
  //Top half of the display
  int fsize = sizeof(frame_buffer);
  for (int i = 0; i < fsize / 2; i += 4) {
    for (int d = 0; d <= 3; d++) {
      //For each of the driver, from 0 to 3
      byte rowData = frame_buffer[i + d];
      mx.setRow(d, d, 7 - int(i / 4), fByte(rowData));
    }
  }
  //Bottom half of the display
  for (int i = fsize / 2; i < fsize; i += 4) {
    for (int d = 4; d <= 7; d++) {
      //For each of the driver, from 4 to 7
      byte rowData = frame_buffer[i + (d - 4)];
      mx.setRow(d, d, 7 - (int(i / 4) - 8), fByte(rowData));
    }
  }
}

//Clear the display to off state
void clearFrame() {
  //Z is reserved for empty screen
  setAnimationCode('z');
}

/* Utilities Functions */
//Set display brightness, from 0x0(min) to 0xF (max)
void setDisplayBrightness(byte brightness) {
  for (int i = 0; i < MAX_DEVICES; i++) {
    mx.control(i, MD_MAX72XX::INTENSITY, brightness);
  }
}

//Helper function to reverse a byte in bits
//e.g. 11011101 -> 10111011
byte fByte(byte c) {
  char r = 0;
  for (byte i = 0; i < 8; i++) {
    r <<= 1;
    r |= c & 1;
    c >>= 1;
  }
  return r;
}
