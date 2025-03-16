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
#define useLogin        // Use HTTP Authentication
#define wifiManager     // Use wifi manager with selection of SSID & IP address
#define useOTA          // Use OTA updates

// =============================================
// LIBRARIES
// =============================================
#include <Arduino.h>
#include <SimpleTimer.h>
#include <Arduino_JSON.h>

// =============================================
// GLOBAL VARIABLES 
// =============================================
// Configure feedback messages for console and BME sensor (Websocket and SSE):
const byte fbkLength = 100;     // Max length of feedback message.
const int aFactor = 10;         // Factor for range of exchanged analog signals (one decimal --> 10; 2 decimals) --> 100. Match with JS!
char feedbackChar[fbkLength];   // Char array to store JSON object to be sent to client.
SimpleTimer timer;              // Setup timers for periodic tasks (websocket clean and take BME readings).
unsigned int BMEtimer;          // Variable to store the timer that runs periodically to retrieve BME readings.
boolean Debug = true;           // true --> enable serial print output
boolean reboot = false;         // Command to reboot the ESP