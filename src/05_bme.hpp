  #include "04_console.hpp"
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
  // Create BME sensor object; connect to ESP I2C (GPIO 21 = SDA, GPIO 22 = SCL)
  Adafruit_BME280 bme;
  // Define NTP Client to get time (used in data file)
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org");  
  unsigned int BMEperiod = 60000L;

  // File name where readings will be saved, and maximum size (bytes)
  const char* dataPath = "/data.txt";
  const int fileLength = 10000U;

  // Function to initialize BME280 sensor
  void initBME(){
    if (!bme.begin(0x76)) { Serial.println(F("BME280 not Found")); while (1);}
    else Serial.println(F("BME280 found"));
  }

  // ===============================================================================
  // Store data from BME sensor in char* "feedbackChar"; JSON format.
  // ===============================================================================
  void readBME(){
    JSONVar jsonObj;                                      // Create JSON object 
    timeClient.update();                                  // Update time from NTP
    jsonObj["time"] = timeClient.getEpochTime();          // time key-value (epochtime in seconds)
    jsonObj["t"]  = int(bme.readTemperature() * aFactor); // Temperature key-value (ºC float 1 decimal to int)
    jsonObj["rh"] = int(bme.readHumidity() * aFactor);    // Humidity key-value (% float 1 decimal to int)
    jsonObj["p"]  = int(bme.readPressure() / 10);         // Pressure key-value (Pascals to DPascals)
    JSON.stringify(jsonObj).toCharArray(feedbackChar, fbkLength);// Return JSON object as char array.
  }

  // ===============================================================================
  // Callback function to be done periodically:
  // ===============================================================================
  void updateBME() { 
    readBME();                                          // Read data from BME sensor
    eventsBME.send("ping", NULL, millis());                // send ping to client
    eventsBME.send(feedbackChar, "newBMEreading", millis());  // Send event "newBMEreading" with last data to client
    if (getFileSize(LittleFS, dataPath) >= fileLength) {
      #ifdef debug 
        Serial.println(F("Deleting big file..."));
      #endif
      deleteFile(LittleFS, dataPath);       // Delete file if too large.
    }
    appendToFile(LittleFS, dataPath, feedbackChar);          // Append new data "feedbackChar" to file 
  }

  void initDataFile() {
    // Create file and add one point if not existing:
    if (!LittleFS.exists(dataPath)) {
      #ifdef debug
        Serial.println(F("Creating file..."));
      #endif
      readBME();                        // Update "feedbackChar" with new readings
      appendToFile(LittleFS, dataPath, feedbackChar);    // Append new data to file
    }
    else {
      #ifdef debug
        Serial.println(F("File exists"));
      #endif
    }
  }

  void initBMErequests() {
    // Receive request for latest sensor readings --> Response HTTP to request received "/refresh"
    server.on("/refresh", HTTP_GET, [](AsyncWebServerRequest *request) {
      readBME();                        // Take one reading
      request->send(200, "application/json", feedbackChar);
      appendToFile(LittleFS, dataPath, feedbackChar);    // Append new data to file
    });

    // Receive request for complete file --> Response HTTP to request received "/data-file"
    server.on("/data-file", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(LittleFS, dataPath, "text/plain");
      // request->send(LittleFS, dataPath, "text/text");
    });

    // Request to delete data file
    server.on("/delete-data", HTTP_GET, [](AsyncWebServerRequest *request) {
      deleteFile(LittleFS, dataPath);
      request->send(200, "text/plain", "data.txt deleted.");
    });
    Serial.println("initBMErequests done");
  }

  void initBMEevents() {
    eventsBME.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        #ifdef debug
          Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
        #endif
      }
      client->send("hello!", NULL, millis(), 1000);
    });
    server.addHandler(&eventsBME);
        Serial.println("initBMEevents done");

  }
#endif

