#include "01_globals.hpp"
#include "02_FileSys.hpp"
#include "03_wifi.hpp"
#include "04_console.hpp"
#include "05_bme.hpp"
#include "06_config.hpp"
#include "07_websocket.hpp"
#include "08_login.hpp"
#include "09_reboot.hpp"

void setup() {
  Serial.begin(115200);

  // ===============================================================================
  // Set GPIO modes and initial values 
  // ===============================================================================
  #ifdef useButton
    initButton();
  #endif
  #ifdef useToggle
    initToggle();
  #endif
  #ifdef usePWM
    initPWM();
  #endif

  // ===============================================================================
  // Initialize all auxiliary systems
  // ===============================================================================
  initFS();         // Initialize file system
  initReboot();     // Initialize periodic check for reboot command

  #if defined(wifiManager)  // Initialize Wifi, optional use wifiManager 
    getWiFi();              // Get SSID, Password and IP from files
    if(!initWiFi()) {       // If SSID or Password were not stored, manage them and reboot
      defineWiFi();
      return; 
    }
  #else
    initWiFi();     // Initialize Wifi with hardcoded values
  #endif

  initWebSocket();  // Initialize WebSocket
  initConfig();     // Initialize Configuration options

  #ifdef useLogin   // Load home page when the server is called (on root "/"). Optional Login
    serverLogin();  
  #else
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/home.html", "text/html",false);
    });
  #endif
  
  #ifdef useBME
    initBME();    // Tasks related with BME sensor
  #endif
  Serial.println(F("Setup done"));
}

void loop() { timer.run(); }