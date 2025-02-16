#include "01_globals.hpp"
#include "02_FileSys.hpp"
#include "03_wifi.hpp"
#include "04_console.hpp"
#include "05_bme.hpp"
#include "06_config.hpp"
#include "07_websocket.hpp"
#ifdef useLogin
  #include "08_login.hpp"
#endif
#include "09_reboot.hpp"

void setup() {
  Serial.begin(115200);

  // ===============================================================================
  // Set GPIO modes and initial values
  // ===============================================================================
  #ifdef useButton
    pinMode(statePin, OUTPUT);
    pinMode(modePin, OUTPUT);
    digitalWrite(statePin, 0);
    digitalWrite(modePin, 1);
  #endif
  #ifdef useToggle
    for (byte i =0; i<n_DOs; i++) { pinMode(arrDO[i], OUTPUT); }
  #endif
  #ifdef usePWM
    for (byte i =0; i<n_PWMs; i++) { 
      pinMode(arrPWM[i][0], OUTPUT);
      analogWrite(arrPWM[i][0], 0);
    }
  #endif

  // ===============================================================================
  // Initialize file system
  // ===============================================================================
  initFS();

  // ===============================================================================
  // Initialize Wifi, optional use wifiManager 
  // ===============================================================================
  #if defined(wifiManager)
    getWiFi();                      // Get SSID, Password and IP from files
    if(!initWiFi()) { defineWiFi(); }  // If SSID or Password were not stored, manage them and reboot
  #else
    initWiFi();                     // Initialize Wifi with hardcoded values
  #endif

  // ===============================================================================
  // Initialize periodic check for reboot command
  // ===============================================================================
  initReboot();

  // ===============================================================================
  // Initialize WebSocket
  // ===============================================================================
  initWebSocket();

  // ===============================================================================
  // Load home page when the server is called (on root "/"). Optional Login
  // ===============================================================================
  #ifdef useLogin
    serverLogin();
  #else
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/home.html", "text/html",false);
    });
  #endif

  // ===============================================================================
  // Initialize Configuration options
  // ===============================================================================
  initConfig();
 
  // ===============================================================================
  // Tasks related with BME sensor
  // ===============================================================================
  #ifdef useBME
    // initNTP();
    initBME();
    initDataFile();    
    initBMErequests();    
    BMEtimer = timer.setInterval(BMEperiod, updateBME);
    initBMEevents();
  #endif
}

void loop() { timer.run(); }