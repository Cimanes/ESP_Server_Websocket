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


// =============================================
// OPTIONS 
// =============================================
#define Toledo      // OPTIONAL: Choose Wifi credentials [Cimanes, Toledo, apartment]
#if defined(Cimanes)
  const char ssid[] = "Pepe_Cimanes";
  const char pass[] = "Cimanes7581" ;
#elif defined(Toledo)
  const char ssid[] = "MIWIFI_HtR7" ;
  const char pass[] = "TdQTDf3H"    ;
#elif defined(apartment)
  const char ssid[] = "HH71VM_309B_2.4G" ;
  const char pass[] = "Ey6DKct3"    ;
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
  Serial.println(WiFi.localIP());
}

// Create AsyncWebServer object on port 80, a WebSocket object ("/wsConsole") and an Event Source ("/eventsBME"):
AsyncWebServer server(80)               ;
AsyncEventSource eventsBME("/eventsBME");   // Required for SERVER SENT EVENTS
AsyncWebSocket wsConsole("/wsConsole")  ;   // Required for WEBSOCKETS
const int cleanTimer = 2000UL           ;   // Timer to periodically clean websocket

