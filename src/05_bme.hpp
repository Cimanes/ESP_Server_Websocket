//  #include "04_console.hpp"
// =============================================
// LIBRARIES
// =============================================
#ifdef useBME
  #include <Adafruit_BME280.h>
  #include <Adafruit_Sensor.h>
  #include <NTPClient.h>

  // =============================================
  // VARIABLES 
  // =============================================
  // Create BME sensor object; connect to ESP I2C (GPIO4 = D2 = SDA, GPIO5 = D1 = SCL)
  Adafruit_BME280 bme;
  // Define NTP Client to get time (used in data file)
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org");  

  // File name where readings will be saved, and maximum size (bytes)
  const char* dataPath = "/data.txt";

  // Configurable variables (via config page):
  unsigned int BMEperiod; // Time interval between BME samples.
  unsigned int fileLimit; // Data-file size limit for BME readings.

  // Function to initialize BME280 sensor
  void initBME(){
    if (!bme.begin(0x76)) { 
      if (Debug) Serial.println(F("BME280 not Found")); 
      while (1);
    }
    else if (Debug) Serial.println(F("BME280 found"));
  }

  // ===============================================================================
  // Store data from BME sensor in char* "feedbackChar"; JSON format.
  // ===============================================================================
  void readBME(){
    JSONVar jsonObj;                              // Create JSON object 
    timeClient.update();                          // wait for time to be updated
    if (Debug) { 
      Serial.println(timeClient.update());        // Print time update to Serial
      Serial.println(timeClient.getFormattedTime()); // Print time to Serial
    }
    jsonObj["time"] = timeClient.getEpochTime();          // time key-value (epochtime in seconds)
    jsonObj["t"]  = int(bme.readTemperature() * aFactor); // Temperature key-value (ºC float 1 decimal to int)
    jsonObj["rh"] = int(bme.readHumidity() * aFactor);    // Humidity key-value (% float 1 decimal to int)
    jsonObj["p"]  = int(bme.readPressure() / 10);         // Pressure key-value (Pascals to mbars)
    JSON.stringify(jsonObj).toCharArray(feedbackChar, fbkLength);// Return JSON object as char array.
    if(Debug) Serial.println(feedbackChar);               // Print JSON object to Serial
    appendToFile(LittleFS, dataPath, feedbackChar);    // Append new data to file
  }

  // ===============================================================================
  // Callback function to be done periodically:
  // ===============================================================================
  void updateBME() { 
    readBME();                                                // Read data from BME sensor
    eventsBME.send("ping", NULL, millis());                   // send ping to client
    eventsBME.send(feedbackChar, "newBMEreading", millis());  // Send event "newBMEreading" with last data to client
    if (getFileSize(LittleFS, dataPath) >= fileLimit) {
      if (Debug)  Serial.println(F("Deleting big file..."));
      deleteFile(LittleFS, dataPath);                         // Delete file if too large.
    }
  }

  void initDataFile() {
    // Create file and add one point if not existing:
    if (!LittleFS.exists(dataPath)) {
      if (Debug)  Serial.println(F("Creating file..."));
      readBME();                        // Update "feedbackChar" with new readings
    }
    else { if (Debug)  Serial.println(F("File exists")); }
  }

  void initBMErequests() {
    // Receive request for latest sensor readings --> re-start timer and take one reading.
    server.on("/refresh", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200);
      timer.deleteTimer(BMEtimer);                        // Stop periodic sampling
      BMEtimer = timer.setInterval(BMEperiod, updateBME); // Restore periodic sampling
      timer.setTimeout(1000,updateBME);                   // Take one reading
      // request->send(200, "application/json", feedbackChar);
    });

    // Receive request for complete file --> Response HTTP to request received "/data-file"
    server.on("/data-file", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, dataPath, "text/plain");
      // request->send(LittleFS, dataPath, "text/text");
    });

    // Request to delete data file
    server.on("/data-file", HTTP_DELETE, [](AsyncWebServerRequest *request) {
      deleteFile(LittleFS, dataPath);
      // request->send(200, "text/plain", "data.txt deleted.");
      request->send(200);
    });
  }

  void initBMEevents() {
    eventsBME.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        if (Debug)  Serial.printf("Client reconnected! Last msg ID: %u\n", client->lastId());
      }
      client->send("hello!", NULL, millis(), 1000);
    });
    server.addHandler(&eventsBME);
  }
#endif

