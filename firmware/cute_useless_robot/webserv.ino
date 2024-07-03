/* Web Server */
const String webroot = "/web/"; //Require tailing slash

// Function to serve files
void handleFileServe(AsyncWebServerRequest *request) {
  String path = request->url();
  String filepath = path;
  filepath.remove(0, 1); // Trim the prefix slash in uri
  filepath = webroot + filepath;
  Serial.println(filepath);
  if (SD.exists(filepath)) {
    request->send(SD, filepath, getMime(filepath));
  } else {
    handleRootRedirect(request);
  }
}

void sendNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "404 - Not found");
}

// Redirect request to index.html
void handleRootRedirect(AsyncWebServerRequest *request) {
  request->redirect("/index.html");
}

//Register all the required API endpoint for web server
void registerAPIEndpoints() {
  /* Basic handlers */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    handleRootRedirect(request);
  });
  server.onNotFound([](AsyncWebServerRequest * request) {
    handleFileServe(request);
  });
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest * request) {
    request->send(200); // Send empty response for the upload URL
  }, handleFileUpload);

  /* Application APIs */
  server.on("/api/ctr/emoji", HTTP_GET, handleRenderEmoji);
  server.on("/api/ctr/move", HTTP_GET, handleMovements);
  server.on("/api/ctr/cover", HTTP_GET, handleCover);
  server.on("/api/ctr/pusher", HTTP_GET, handlePusher);
  server.on("/api/fs/listDir", HTTP_GET, handleListDir);
  server.on("/api/fs/download", HTTP_GET, handleFileDownload);
  server.on("/api/fs/delete", HTTP_GET, handleFileDelete);
  server.on("/api/ipaddr", HTTP_GET, handleGetIPAddress);
}
//ip addr
void handleGetIPAddress(AsyncWebServerRequest *request) {
  String ipAddress;
  if (!ENABLE_WIFI_DEBUG) {
    ipAddress = WiFi.softAPIP().toString();
  } else {
    ipAddress = WiFi.localIP().toString();
  }
  request->send(200, "application/json", "{\"ip\":\"" + ipAddress + "\"}");
}

/* Utilities */
String GetPara(AsyncWebServerRequest *request, String key) {
  if (request->hasParam(key)) {
    return request->getParam(key)->value();
  }
  return "";
}


//Get the filename from filepath
String basename(const String& filePath) {
  int lastSlashIndex = filePath.lastIndexOf('/');

  // If no slash is found, return the original path
  if (lastSlashIndex == -1) {
    return filePath;
  }

  // Return the substring after the last slash
  return filePath.substring(lastSlashIndex + 1);
}

String getMime(const String& path) {
  String _contentType = "text/plain";
  if (path.endsWith(".html")) _contentType = "text/html";
  else if (path.endsWith(".htm")) _contentType = "text/html";
  else if (path.endsWith(".css")) _contentType = "text/css";
  else if (path.endsWith(".json")) _contentType = "text/json";
  else if (path.endsWith(".js")) _contentType = "application/javascript";
  else if (path.endsWith(".png")) _contentType = "image/png";
  else if (path.endsWith(".gif")) _contentType = "image/gif";
  else if (path.endsWith(".jpg")) _contentType = "image/jpeg";
  else if (path.endsWith(".ico")) _contentType = "image/x-icon";
  else if (path.endsWith(".svg")) _contentType = "image/svg+xml";
  else if (path.endsWith(".eot")) _contentType = "font/eot";
  else if (path.endsWith(".woff")) _contentType = "font/woff";
  else if (path.endsWith(".woff2")) _contentType = "font/woff2";
  else if (path.endsWith(".ttf")) _contentType = "font/ttf";
  else if (path.endsWith(".xml")) _contentType = "text/xml";
  else if (path.endsWith(".pdf")) _contentType = "application/pdf";
  else if (path.endsWith(".zip")) _contentType = "application/zip";
  else if (path.endsWith(".gz")) _contentType = "application/x-gzip";
  else if (path.endsWith(".mp3")) _contentType = "audio/mpeg";
  else if (path.endsWith(".mp4")) _contentType = "video/mp4";
  else if (path.endsWith(".aac")) _contentType = "audio/aac";
  else if (path.endsWith(".ogg")) _contentType = "audio/ogg";
  else if (path.endsWith(".wav")) _contentType = "audio/wav";
  else if (path.endsWith(".m4v")) _contentType = "video/x-m4v";
  else if (path.endsWith(".webm")) _contentType = "video/webm";
  else _contentType = "text/plain";
  return _contentType;
}
