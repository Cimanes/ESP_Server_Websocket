  #include "03_console.hpp"
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
  const int BMETimer = 30000UL;

  // File name where readings will be saved, and maximum size (bytes)
  const char* dataPath = "/data.txt";
  const int fileLength = 10000U;

  // Function to initialize BME280 sensor
  void initBME(){
    if (!bme.begin(0x76)) { Serial.println(F("BME280 not Found")); while (1);}
    else Serial.println(F("BME280 found"));
  }

  // ===============================================================================
  // Read file from LittleFS (only used for debug in Serial.print)
  // ===============================================================================
  #ifdef debug
   const char* readFile(fs::FS &fs, const char * path) {
      Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println(F("- file not found"));
      return nullptr;
    }
    // Allocate memory for the file content + null terminator
    char* fileContent = new char[file.size() + 1]; 
    // Read the file content
    if (file.readBytes(fileContent, file.size()) != file.size()) {
        Serial.println(F("- error reading file"));
      delete[] fileContent; // Free memory
      file.close();
      return nullptr;
    }
    fileContent[file.size()] = '\0';  // Null-terminate the string
    file.close();
    return fileContent;
  }
  #endif 

  // ===============================================================================
  // Get size from file in LittleFS
  // ===============================================================================
  const unsigned int getFileSize(fs::FS &fs, const char * path){
    File file = fs.open(path, "r");
    if(!file){
      #ifdef debug
        Serial.println(F("Failed to open file for checking size"));
      #endif
      return 0;
    }
    #ifdef debug
      Serial.print(F("File size: "));
      Serial.println(file.size());
    #endif
    return file.size();
  }

  // ===============================================================================
  // Append data to file in LittleFS
  // ===============================================================================
  void BMEtoFile(fs::FS &fs, const char * path){
    #ifdef debug
      Serial.printf("Appending to file: %s\r\n", path);
    #endif
    File file = fs.open(path, "a");
    if(!file){
      #ifdef debug 
        Serial.println(F("- failed to open file for appending"));
      #endif
      return;
    }
    strncat(feedbackChar, ",", 1);    // Add "," at the end of "feedbackChar"
    if(file.print(feedbackChar)) {
      #ifdef debug
        Serial.println(F("- msg. appended"));
      #endif
    }
    // Remove the comma by replacing it with a null terminator    
    const byte len = strlen(feedbackChar);
    if (len > 0 && feedbackChar[len - 1] == ',') { feedbackChar[len - 1] = '\0'; }
    #ifdef debug
      else Serial.println(F("- append failed"));  
    #endif    
    file.close();
  }

  // ===============================================================================
  // Delete File from LittleFS
  // ===============================================================================
  void deleteFile(fs::FS &fs, const char * path){
    #ifdef debug
      Serial.printf("Deleting file: %s\r\n", path);
    #endif
    if(fs.remove(path)) {
      #ifdef debug
        Serial.println(F("- file deleted"));
      #endif
    }
    else {
      #ifdef debug
        Serial.println(F("- delete failed"));
      #endif
    }  
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
    events.send("ping", NULL, millis());                // send ping to client
    events.send(feedbackChar, "BMEreading", millis());  // Send event "BMEreading" with last data to client
    if (getFileSize(LittleFS, dataPath) >= fileLength) {
      #ifdef debug 
        Serial.println(F("Deleting big file..."));
      #endif
      deleteFile(LittleFS, dataPath);       // Delete file if too large.
    }
    BMEtoFile(LittleFS, dataPath);          // Append new data "feedbackChar" to file 
  }
#endif