#include "04_bme.hpp"

void setup() {
  Serial.begin(115200);

  // ===============================================================================
  // Set GPIO modes and initial values
  // ===============================================================================
  #ifdef useButton
    pinMode(statePin, OUTPUT);
    pinMode(modePin, OUTPUT);
    digitalWrite(statePin, 0);
    digitalWrite(modePin, 0);
  #endif
  #ifdef useToggle
    for (byte i =0; i<numDOs; i++) { pinMode(arrDO[i], OUTPUT); }
  #endif
  #ifdef usePWM
    for (byte i =0; i<numPWMs; i++) { 
      pinMode(arrPWM[i][0], OUTPUT);
      analogWrite(arrPWM[i][0], 0);
    }
  #endif

  // ===============================================================================
  // Initialize wifi, file system, WebSocket and setup timer: 
  // ===============================================================================
  initWiFi();
  initFS();
  initWebSocket();
  timer.setInterval(cleanTimer, clean);

  // ===============================================================================
  // Load index page when the server is called (on root "/")
  // ===============================================================================
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });
  
  // ===============================================================================
  // Tasks related with BME sensor
  // ===============================================================================
  #ifdef useBME       
    initBME();
    // Create file and add one point if not existing:
    if (!LittleFS.exists(dataPath)) {
      #ifdef debug
        Serial.println(F("Creating file..."));
      #endif
      readBME();                        // Update "feedbackChar" with new readings
      BMEtoFile(LittleFS, dataPath);    // Append new data to file
    }
    else {
      #ifdef debug
        Serial.println(F("File exists"));
      #endif
    }
    timer.setInterval(BMETimer, updateBME);

    // Receive request for latest sensor readings --> Response HTTP to request received "/refresh"
    server.on("/refresh", HTTP_GET, [](AsyncWebServerRequest *request) {
      readBME();
      
      request->send(200, "application/json", feedbackChar);
      // request->send(200);
      // updateBME();
    });

    // Receive request for complete file --> Response HTTP to request received "/data-file"
    server.on("/data-file", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, dataPath, "text/text");
    });

    // Request to delete data file
    server.on("/delete-data", HTTP_GET, [](AsyncWebServerRequest *request) {
      deleteFile(LittleFS, dataPath);
      request->send(200, "text/plain", "data.txt deleted.");
    });

    // On connection, send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    events.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        #ifdef debug
          Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
        #endif
      }
      client->send("hello!", NULL, millis(), 1000);
    });

    server.addHandler(&events);
  #endif

  // Serve files (JS, CSS and favicon) in a directory when requested by the root URL. 
  server.serveStatic("/", LittleFS, "/");
  // Start the server.
  server.begin();   
}

void loop() {
  timer.run();
}
