#include "01_globals.hpp"
#include "02_FileSys.hpp"
#include "03_wifi.hpp"
#include "04_console.hpp"
#include "05_bme.hpp"
#include "06_config.hpp"
#include "07_login.hpp"

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
  // Initialize wifi, file system, WebSocket and setup timer: 
  // ===============================================================================
  initWiFi();
  initFS();
  initWebSocket();
  timer.setInterval(cleanTimer, clean);

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

  
  #ifdef useConfig
    initConfig();
  #endif
  
  // ===============================================================================
  // Tasks related with BME sensor
  // ===============================================================================
  #ifdef useBME       
    initBME();
    initDataFile();
    initBMErequests();
    BMEtimer = timer.setInterval(BMEperiod, updateBME);
    initBMEevents();
  #endif

  // Serve files (JS, CSS and favicon) from LittleFS when requested by the root URL. 
  server.serveStatic("/", LittleFS, "/");
  // Start the server.
  server.begin();
}

void loop() {
  timer.run();
}
