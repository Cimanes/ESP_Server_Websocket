#include "01_globals.hpp"

// =============================================
// MANAGE FILE SYSTEM
// =============================================
void initFS() {
  if (!LittleFS.begin()) Serial.println(F("Error mounting File System"));
  // if (!SPIFFS.begin(true)) Serial.println("Error mounting File System");      // particular for SPIFFS in ESP32 only
  else Serial.println(F("File System mounted"));
}

// ===============================================================================
// Read file from LittleFS into const char*
// ===============================================================================
// const char* readFile(fs::FS &fs, const char * path) {
//   #ifdef debug 
//     Serial.printf("Reading file: %s\r\n", path);
//   #endif 
//   File file = fs.open(path, "r");
//   #ifdef debug 
//     if(!file || file.isDirectory()){
//         Serial.println(F("- file not found"));
//       return nullptr;
//     }
//     // Allocate memory for the file content + null terminator
//     char* fileContent = new char[file.size() + 1]; 
//     // Read the file content: 
//     if (file.readBytes(fileContent, file.size()) != file.size()) {
//         Serial.println(F("- error reading file"));
//       delete[] fileContent; // Free memory
//       file.close();
//       return nullptr;
//     }
//   #endif
//   fileContent[file.size()] = '\0';  // Null-terminate the string
//   file.close();
//   return fileContent;
// }

// ===============================================================================
// Read file from LittleFS into char* variable
// ===============================================================================
void fileToCharPtr(fs::FS &fs, const char * path, char * buffer) {
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    Serial.println("no file");
    strncpy(buffer, "", sizeof(buffer));
    return;
  }
  Serial.println("Yes file");
  size_t i = 0;
  while (file.available() && i < sizeof(buffer) - 1) {
    buffer[i++] = (char)file.read();
  }
  buffer[i] = '\0';
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
// Write file to LittleFS
// ===============================================================================
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// ===============================================================================
// Append data to file in LittleFS
// ===============================================================================
void appendToFile(fs::FS &fs, const char * path, const char * message) {
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
  if(file.print(message) && file.print(",")) {
    #ifdef debug
      Serial.println(F("- msg. appended"));
    #endif
  }
  #ifdef debug
    else Serial.println(F("- append failed"));  
  #endif    
  file.close();
}
