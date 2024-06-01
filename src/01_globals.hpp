// =============================================
// OPTIONS 
// =============================================
#define useButton       // Use buttons
#define useBVAR         // Use Boolean control variables
#define useToggle       // Use toggle switches (ON - OFF)
#define usePWM          // Use analog output channels (PWM's)
#define useAVAR         // Use floating control variables
#define useBME          // Use BME sensor
#define useInputs       // Use input fields (text & numbers using POST method)
#define debug           // for debugging purpose only. Remove for final version.

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
  #define sdaPin 4
  #define sclPin 5
#endif

// =============================================
// GLOBAL VARIABLES 
// =============================================
// Configure feedback messages for console and BME sensor (Websocket and SSE):
const byte fbkLength = 60;        // Max length of feedback message
const int aFactor = 10;           // Factor for range of analog signals (10 -> one decimal; 100 -> 2 decimals). Match with JS!
char feedbackChar[fbkLength];     // Char array to store JSON object to be sent to client.
SimpleTimer timer;                // Setup timers for periodic tasks (websocket clean and take BME readings):
int BMEtimer;                     // variable to store the timer that runs periodically to retrieve BME readings.




