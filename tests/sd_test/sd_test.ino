#include <SPI.h>
#include <SD.h>

#define CS_PIN 5

void setup() {
  Serial.begin(115200);
  
  // Initialize SD card
  if (!SD.begin(CS_PIN)) {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.println("Card Initialized");

  // Create and open file
  File file = SD.open("/HelloWorld.txt", FILE_WRITE);
  
  // Check if the file opened successfully
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  
  // Write to file
  file.println("Hello World!");
  
  // Close the file
  file.close();
  
  Serial.println("File written successfully");
}

void loop() {
  // Nothing to do here
}
