// =============================================
// VARIABLES 
// =============================================
unsigned int rebootPeriod = 1000; // Time interval to check for restart required.
unsigned int rebootTimer;

// =============================================
// Periodic function to check if a reboot is requested
// =============================================
void checkReboot() {
  if(reboot) { 
    timer.setTimeout(5000, []() {ESP.restart();} ); 
    reboot = false;
  }
}

void initReboot() { // Request to delete data file
  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    reboot = true;
    request->send(200);
  });
  rebootTimer = timer.setInterval(rebootPeriod, checkReboot);
}

