// =============================================
// VARIABLES
// =============================================
  FSInfo fs_info;             // FSInfo is a structure (defined in LittleFS library) that holds information about the file system
  const byte paramSize = 25;  // Maximum size for SSID, Password and IP
  
// =============================================
// MANAGE FILE SYSTEM
// =============================================
void initFS() {
  if (!LittleFS.begin()) {
    if (Debug) Serial.println(F("Error mounting File System"));
  }
  // if (!SPIFFS.begin(true)) Serial.println("Error mounting File System");      // particular for SPIFFS in ESP32 only
  else {
    if (Debug) Serial.println(F("File System mounted"));
    LittleFS.info(fs_info);                                   // Populates fs_info structure with info about LittleFS
    totalBytes = fs_info.totalBytes/1000;  // Total memory in LittleFS
  }
}

// ===============================================================================
// Read file from LittleFS into const char*
// ===============================================================================
// const char* readFile(fs::FS &fs, const char * path) {
//   if (Debug) Serial.printf("Reading file: %s\r\n", path);
//   File file = fs.open(path, "r");
//   if (Debug) 
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
//   fileContent[file.size()] = '\0';  // Null-terminate the string
//   file.close();
//   return fileContent;
// }

// ===============================================================================
// Read file from LittleFS into char* variable
// ===============================================================================
void fileToCharPtr(fs::FS &fs, const char* path, char* buffer) {
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    if (Debug) Serial.println("no file");
    // strncpy(buffer, "", strlen(buffer));
    buffer[0] = '\0'; // Ensure the buffer is null-terminated
    return;
  }
  if (Debug) Serial.println("Yes file");
  size_t i = 0;
  while (file.available() && i < paramSize - 1) {
    buffer[i++] = (char)file.read();
  }
  buffer[i] = '\0';
  file.close();
}

// ===============================================================================
// Delete File from LittleFS
// ===============================================================================
void deleteFile(fs::FS &fs, const char * path){
  if (Debug)  Serial.printf("Deleting file: %s\r\n", path);
  if(fs.remove(path)) {
    if (Debug)  Serial.println(F("- file deleted"));
  }
  else {
    if (Debug)  Serial.println(F("- delete failed"));
  }  
}

// ===============================================================================
// Get size from data-file in LittleFS
// ===============================================================================
const unsigned int getFileSize(fs::FS &fs, const char * path){
  File file = fs.open(path, "r");
  if(!file){
    if (Debug)  Serial.println(F("Failed to open file for checking size"));
    return 0;
  }
  if (Debug) {
    Serial.print(F("File size: "));
    Serial.println(file.size());
  }
  return file.size();
}

// ===============================================================================
// Write file to LittleFS
// ===============================================================================
void writeFile(fs::FS &fs, const char * path, const char * message){
  if (Debug) Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    if (Debug) Serial.println(F("- failed to open file for writing"));
    return;
  }
  if(file.print(message)) {
    if (Debug) Serial.println(F("- file written"));
    return;
  }
  else { 
    if (Debug) Serial.println(F("- write failed"));
    return;
  }
}

// ===============================================================================
// Append data to file in LittleFS
// ===============================================================================
void appendToFile(fs::FS &fs, const char * path, const char * message) {
  if (Debug)  Serial.printf("Appending to file: %s\r\n", path);
  File file = fs.open(path, "a");
  if(!file){
    if (Debug)  Serial.println(F("- failed to open file for appending"));
    return;
  }
  if(file.print(message) && file.print(",")) {
    if (Debug)  Serial.println(F("- msg. appended"));
  }
  else { 
    if (Debug) Serial.println(F("- append failed")); 
  }
  file.close();
}