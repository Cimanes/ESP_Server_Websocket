#include "06_inputs.hpp"

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
    for (byte i =0; i<numDOs; i++) { pinMode(arrDO[i], OUTPUT); }
  #endif
  #ifdef usePWM
    for (byte i =0; i<numPWMs; i++) { 
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
  // Load index page when the server is called (on root "/")
  // ===============================================================================
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });
  
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

  #ifdef useInputs
    initInputs();
  #endif


  // Serve files (JS, CSS and favicon) from LittleFS when requested by the root URL. 
  server.serveStatic("/", LittleFS, "/");
  // Start the server.
  server.begin();   
}

void loop() {
  timer.run();
}
