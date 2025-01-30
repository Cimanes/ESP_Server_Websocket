// =============================================
// OPTIONS 
// =============================================
#define useButton       // Use buttons
#define useToggle       // Use toggle switches (ON - OFF)
#define usePWM          // Use analog output channels (PWM's)
#define useBVAR         // Use Boolean control variables
#define useAVAR         // Use floating control variables
#define useTxIn         // Use text input fields
#define useNumIn        // Use numeric input fields (tune BMEperiod and data-file size limit)
#define useFbk          // Use Feedback fields (receive usedBytes and totalBytes)
#define useAVAR         // Use floating control variables
#define useBME          // Use BME sensor
#define useConfig       // Use input fields (text & numbers using POST method)

// =============================================
// LIBRARIES
// =============================================
#include <Arduino.h>
#include <SimpleTimer.h>
#include <Arduino_JSON.h>
#if defined(ESP32)
  #include "SPIFFS.h"         // OPTIONAL: available for SPIFFS in ESP32 only
#elif defined(ESP8266)
  #include <LittleFS.h>       // OPTIONAL: Little file system for ESP8266
#endif

// =============================================
// PIN DEFINITIONS 
// =============================================
#if defined(ESP32)
  #define sdaPin 21
  #define sclPin 22
#elif defined(ESP8266)
  #define sdaPin 4  // D2 in Wemos D1 Mini
  #define sclPin 5  // D1 in Wemos D1 Mini
#endif

// =============================================
// GLOBAL VARIABLES 
// =============================================
// Configure feedback messages for console and BME sensor (Websocket and SSE):
const byte fbkLength = 100;      // Max length of feedback message.
const int aFactor = 10;         // Factor for range of exchanged analog signals (10 -> one decimal; 100 -> 2 decimals). Match with JS!
char feedbackChar[fbkLength];   // Char array to store JSON object to be sent to client.
SimpleTimer timer;              // Setup timers for periodic tasks (websocket clean and take BME readings).
int BMEtimer;                   // Variable to store the timer that runs periodically to retrieve BME readings.
unsigned int usedBytes;         // Used memory in FS system.
unsigned int totalBytes;        // Total memory in FS system.
boolean Debug = true;           // true --> serial print output

// Web Server HTTP Authentication credentials
// const char* http_username = "admin";
// const char* http_password = "admin";