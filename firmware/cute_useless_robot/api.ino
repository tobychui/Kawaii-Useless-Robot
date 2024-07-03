/* Control API for Robot motion system */
//Require direction (up / down / left / right)
//Requite state (start / stop)
void handleMovements(AsyncWebServerRequest *request) {
  String direction = GetPara(request, "direction");
  String state = GetPara(request, "state");
  
  //Set current moving direciton
  if (direction == "up"){
    movingDirection = 0;
  }else if (direction == "down"){
    movingDirection = 1;
  }else if (direction == "left"){
    movingDirection = 2;
  }else if (direction == "right"){
    movingDirection = 3;
  }else{
    request->send(500, "text/plain", "500 - Invalid direction given");
    return;
  }

  if (state == "start"){
    //Start
    movingActive = true;
  }else{
    //Stop
    movingActive = false;
  }

  request->send(200, "text/plain", "ok");
}

//Handle request for moving the pushing arm, require angle (range 0 - 130)
void handlePusher(AsyncWebServerRequest *request){
  String angle = GetPara(request, "angle");
  //Angle must be within 0 - 130 (+ offset)
  int limitedAngle = constrain(angle.toInt(), 0, 130 + SERVO_ALIGNMENT_OFFSET);
  servoSwitchPusher.write(limitedAngle);
  request->send(200, "text/plain", "ok");
}

//Handle request for moving the cover, require state: {open / close}
void handleCover(AsyncWebServerRequest *request){
  String state = GetPara(request, "state");
  if (state == "open"){
    //Open
    servoCoverPusher.write(90);
  }else{
    //Close
    servoCoverPusher.write(0);
  }
  request->send(200, "text/plain", "ok");
}

//Render emoji to screen
void handleRenderEmoji(AsyncWebServerRequest *request) {
  String anicode = GetPara(request, "anicode");
  char anicodeChar = anicode.c_str()[0];
  setAnimationCode(anicodeChar);
  request->send(200, "text/plain", "ok");
}

/* File System API for SD Browser*/

//File delete API
void handleFileDelete(AsyncWebServerRequest *request) {
  String path = GetPara(request, "path");
  if (path == "") {
    request->send(400, "text/plain", "Missing 'path' parameter");
    return;
  }
  Serial.print("Requested delete path: ");
  Serial.println(path);

  if (SD.exists(path)) {
    if (SD.remove(path)) {
      request->send(200, "text/plain", "File removed");
    } else {
      request->send(500, "text/plain", "Failed to delete file");
    }
  } else {
    request->send(404, "text/plain", "File not found");
  }
}

//File download API
void handleFileDownload(AsyncWebServerRequest *request) {
  String path = GetPara(request, "path");
  if (path == "") {
    request->send(404, "text/plain", "'path' parameter not given");
    return;
  }
  Serial.print("Requested path: ");
  Serial.println(path);
  if (SD.exists(path)) {
    String contentType = getMime(path);
    request->send(SD, path, contentType, false);
  } else {
    request->send(404, "text/plain", "File not found");
  }
}

//List dir API
void handleListDir(AsyncWebServerRequest *request) {
  //Get the folder path to be listed
  //As ESP8266 dont have enough memory for proper struct to json conv, we are hacking a json string out of a single for-loop
  String jsonString = "[";
  String folderSubPath = GetPara(request, "dir");
  String folderPath = "/" + folderSubPath;
  if (SD.exists(folderPath)) {
    File root = SD.open(folderPath);
    bool firstObject = true;
    if (root) {
      while (true) {
        File entry = root.openNextFile();
        if (!entry) {
          // No more files
          break;
        } else {
          //There are more lines. Add a , to the end of the previous json object
          if (!firstObject) {
            jsonString = jsonString + ",";
          } else {
            firstObject = false;
          }
        }

        String isDirString = "true";
        if (!entry.isDirectory()) {
          isDirString = "false";
        }

        jsonString = jsonString + "{\"Filename\":\"" + entry.name() + "\",\"Filesize\":" + String(entry.size()) + ",\"IsDir\":" + isDirString + "}";
        entry.close();
      }
      root.close();

      jsonString += "]";
      request->send(200, "application/json", jsonString);
    } else {
      request->send(500, "text/plain", "500 - Path open error");
    }
  } else {
    request->send(404, "text/plain", "404 - Path not found");
  }
  Serial.println(folderPath);
}

// Function to handle file uploads
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  static File uploadFile;
  static String uploadPath = "/";

  if (request->hasParam("dir")) {
    uploadPath = "/" + request->getParam("dir")->value() + "/";
  }

  if (index == 0) {
    String path = uploadPath + filename;
    Serial.printf("Upload Start: %s\n", path.c_str());
    uploadFile = SD.open(path, FILE_WRITE);
    if (!uploadFile) {
      Serial.println("Failed to open file for writing");
      return request->send(500, "text/plain", "File Upload Failed");
    }
  }

  // Write the received data to the file
  if (uploadFile) {
    uploadFile.write(data, len);
  }

  if (final) {
    // Close the file at the final chunk
    if (uploadFile) {
      uploadFile.close();
      Serial.printf("Upload End: %s (%u)\n", filename.c_str(), index + len);
      return request->send(200, "text/plain", "File Uploaded");
    } else {
      return request->send(500, "text/plain", "File Upload Failed");
    }
  }
}
