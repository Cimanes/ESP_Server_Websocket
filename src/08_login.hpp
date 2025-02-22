// =============================================
// VARIABLES
// =============================================
const char* http_username = "admin";
const char* http_password = "admin";


// =============================================
// Setup the login page
// =============================================
void serverLogin() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!request->authenticate(http_username, http_password)) {
      return request->requestAuthentication(); 
    }
    request->send(LittleFS, "/home.html", "text/html",false);
  });
  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/logged-out.html", "text/html",false);
  });  
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(401);     // Unauthorized
  });
  server.serveStatic("/", LittleFS, "/").setAuthentication(http_username, http_password);
  Serial.println(F("ServerLogin done"));
}