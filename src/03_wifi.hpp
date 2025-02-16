// =============================================
// LIBRARIES
// =============================================
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#if defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <AsyncElegantOTA.h>
// =============================================
// VARIABLES
// =============================================
// Create AsyncWebServer object on port 80, a WebSocket object ("/wsConsole") and an Event Source ("/eventsBME"):
AsyncWebServer server(80)               ;   // Required for HTTP 
AsyncEventSource eventsBME("/eventsBME");   // Required for SERVER SENT EVENTS
AsyncWebSocket wsConsole("/wsConsole")  ;   // Required for WEBSOCKETS
const int cleanTimer = 2000UL           ;   // Timer to periodically clean websocket

#if defined(wifiManager)
  // =============================================
  // Wifi Manager: set SSID, Password and IP address
  // =============================================
  // Search for parameter in HTTP POST request
  const char* PARAM_INPUT_1 = "ssid";
  const char* PARAM_INPUT_2 = "pass";
  const char* PARAM_INPUT_3 = "ip";
  const char* PARAM_INPUT_4 = "router";
  // const char* PARAM_INPUTS[4] = { "ssid", "pass", "ip", "router" };

  //Variables to save values from HTML form
  char ssid[paramSize];
  char pass[paramSize];
  char ip[paramSize];
  char router[paramSize];

  // File paths to save input values permanently
  const char* ssidPath = "/ssid.txt";
  const char* passPath = "/pass.txt";
  const char* ipPath = "/ip.txt";
  const char* routerPath = "/router.txt";
  // const char* paramPaths[4] = { "/ssid.txt", "/pass.txt", "/ip.txt", "/router.txt" };

  // Function to Initialize Wifi
  bool initWiFi() {
    if(strcmp(ssid, "") == 0 || strcmp(ip, "") == 0 || strcmp(router, "") == 0 ){
      Serial.println(F("Undefined WiFi"));
      return false;
    }  
    
    // Option: required for static IP address
    //------------------------------------------------------------------------
    IPAddress subnet(255, 255, 0, 0);
    // IPAddress localIP(192, 168, 1, 200); // Option: Hard coded IP address
    // IPAddress gateway(192, 168, 1, 254); // Option: Hard coded gateway
    // IPAddress dns(192, 168, 1, 254);     // Option: Hard coded IP address
    IPAddress localIP;
    IPAddress gateway;
    IPAddress dns;    

    localIP.fromString(ip);
    gateway.fromString(router);
    dns.fromString(router);

    if (!WiFi.config(localIP, gateway, subnet, dns)){
      if (Debug) Serial.println(F("STA config Failed"));
      return false;
    }
    //------------------------------------------------------------------------

    WiFi.mode(WIFI_STA);   
    WiFi.begin(ssid, pass);

    Serial.print(F("Connecting .."));
    while (WiFi.status() != WL_CONNECTED) { 
      Serial.print('.'); delay(1000);
    }
    if (Debug) Serial.println(WiFi.localIP());
    // Serve files (JS, CSS and favicon) from LittleFS when requested by the root URL. 
    server.serveStatic("/", LittleFS, "/");
    #ifdef OTA
      AsyncElegantOTA.begin(&server); // Start OTA
    #endif
    server.begin(); // Start the server.    
    return true;
  }

  void getWiFi() {
    fileToCharPtr(LittleFS, ssidPath, ssid);      // Search for stored SSID
    fileToCharPtr(LittleFS, passPath, pass);      // Search for stored Password
    fileToCharPtr(LittleFS, ipPath, ip);          // Search for stored local IP
    fileToCharPtr(LittleFS, routerPath, router);  // Search for stored router IP
  }



  void defineWiFi() {    // Connect to ESP Wi-Fi network with SSID and password
    Serial.println(F("Setting AP")); 
    // Remove the password parameter (=NULL), if you want the AP (Access Point) to be open 
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print(F("AP local IP: "));
    Serial.println(IP);

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html");
    });
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        const AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            strncpy(ssid, p->value().c_str(), paramSize - 1);
            ssid[paramSize - 1] = '\0'; // Ensure null-termination
            Serial.print(F("SSID: "));  Serial.println(ssid);
            writeFile(LittleFS, ssidPath, ssid);
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            strncpy(pass, p->value().c_str(), paramSize - 1);
            pass[paramSize - 1] = '\0'; // Ensure null-termination
            Serial.print(F("Password: "));  Serial.println(pass);
            writeFile(LittleFS, passPath, pass);
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            strncpy(ip, p->value().c_str(), paramSize - 1);
            ip[paramSize - 1] = '\0'; // Ensure null-termination
            Serial.print(F("Local IP: "));  Serial.println(ip);
            writeFile(LittleFS, ipPath, ip);
          }
          // HTTP POST router IP value
          if (p->name() == PARAM_INPUT_4) {
            strncpy(router, p->value().c_str(), paramSize - 1);
            ip[paramSize - 1] = '\0'; // Ensure null-termination
            Serial.print(F("Router IP: "));
            Serial.println(router);
            writeFile(LittleFS, routerPath, router);
          } 
          Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str()); 
        }
      }
      reboot = true;
      request->send(200, "text/plain", "Done. ESP rebooting, connect to your router. ESP IP address: " + String(ip));
    });
    
    // Serve files (JS, CSS and favicon) from LittleFS when requested by the root URL. 
    server.serveStatic("/", LittleFS, "/");
    #ifdef OTA
      AsyncElegantOTA.begin(&server); // Start OTA
    #endif
    server.begin(); // Start the server.
  }
#else
  // =============================================
  // Hardcoded Wifi credentials 
  // =============================================
  #define apartment      // OPTIONAL: Choose Wifi credentials [Cimanes, Toledo, apartment]
  #if defined(Cimanes)
    const char ssid[] = "Pepe_Cimanes";
    const char pass[] = "Cimanes7581" ;
  #elif defined(Toledo)
    const char ssid[] = "MIWIFI_HtR7" ;
    const char pass[] = "TdQTDf3H"    ;
  #elif defined(apartment)
    const char ssid[] = "John-Rs-Foodhall_EXT" ;
    const char pass[] = "sive2017"    ;
  #endif

  // Function to Initialize Wifi
  void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.print(F("Connecting to WiFi .."));
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    }
    // Serve files (JS, CSS and favicon) from LittleFS when requested by the root URL. 
    server.serveStatic("/", LittleFS, "/");
    #ifdef OTA
      AsyncElegantOTA.begin(&server); // Start OTA
    #endif
    server.begin();                   // Start the server.
    Serial.println(WiFi.localIP());
  }
#endif
