// ===============================================================================
// LIBRARIES
// ===============================================================================
#include <Arduino.h>
#include <SimpleTimer.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
// #define ESP8266            // OPTIONAL: define type board family: [ESP8266, ESP32]
#if defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include "SPIFFS.h"         // OPTIONAL: available for SPIFFS in ESP32 only
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <LittleFS.h>       // OPTIONAL: Little file system for ESP8266
#endif

// ===============================================================================
// OPTIONS 
// ===============================================================================
#define useButton       // Use buttons
#define useBVAR         // Use Boolean control variables
#define useToggle       // Use toggle switches (ON - OFF)
#define usePWM          // Use analog output channels (PWM's)
#define useAVAR         // Use floating control variables
#define debug           // for debugging purpose only. Remove for final version.
#define Toledo          // OPTIONAL: Choose Wifi credentials [Cimanes, Toledo, apartment]
#define aFactor 10      // Factor for range of analog signals (10 -> one decimal; 100 -> 2 decimals). Match with JS!


// ===============================================================================
// MANAGE FILE SYSTEM AND COMMUNICATIONS 
// ===============================================================================
// Create AsyncWebServer object on port 80 and WebSocket object:
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Setup periodic "cleanClients":
int cleanTimer = 2000UL;
SimpleTimer timer;
void clean() { ws.cleanupClients();}

// Function to Initialize Wifi
void initWiFi() {
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
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print(F("Connecting to WiFi .."));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

// Function to Initialize File System
void initFS() {
  if (!LittleFS.begin()) Serial.println(F("Error mounting File System"));
  // if (!SPIFFS.begin(true)) Serial.println("Error mounting File System");      // particular for SPIFFS in ESP32 only
  else Serial.println(F("File System mounted OK"));
}

// Function to notify all clients with a message containing the argument
void notifyClients(String msg) { ws.textAll(msg); }

// ===============================================================================
// Variables and functions defined to deal with buttons
// ===============================================================================
#ifdef useButton
  // Set output GPIO's used by the buttons (ON/OFF = GPIO2, AUTO/MAN = GPIO4 )
  #define statePin 2
  #define modePin 4
 
  // function "updateButton()": Replace placeholders found in HTML (%STATE%, %MODE%...) with their current value
  // Pass argument by reference "&var", so we can change its value inside the function:
  String updateButton(const String &var) {
   String feedback;
    if(var == "STATE") {
      if(digitalRead(statePin)) feedback = "ON";
      else feedback = "OFF";
    }
    else if (var == "MODE") {
      if(digitalRead(modePin)) feedback = "AUTO";
      else feedback = "MAN";
    }
    return feedback;
  }
#endif

// ===============================================================================
// TOGGLE SWITCHES (Digital outputs): Variables and functions
// ===============================================================================
#ifdef useToggle
  // Define the Digital Outputs to be controlled via toggle switches
    #define numDOs 2
    const byte arrDO[numDOs] = {12, 14};
  // Update toggle switch: return JSON object {"dfb":12, "state":1}
  String updateDO(byte gpio){
    JSONVar jsonObj;
    jsonObj["dfb"] = gpio;                  // Number of the GPIO
    jsonObj["state"] = digitalRead(gpio);   // 0 or 1
    return JSON.stringify(jsonObj);         // JSON object converted into a String.
  }
#endif

// ===============================================================================
// CONTROL BOOLEAN VARIABLES: Variables and functions
// ===============================================================================
#ifdef useBVAR
  // Define the PWM output channels and ranges
  #define numBVARS 2
  const char* BVAR[numBVARS] = {"bVAR1", "bVAR2"};    // Array with boolean variable names 
  bool BVARval[numBVARS] = {0, 0};                    // Array to store boolean variable values

  // Update boolean feedback of control variable: return JSON object {"dfb":"bVAR1", "state":0}
  String updateBVAR(byte index){
    JSONVar jsonObj;                      // Create JSON object for Floating Variables
    jsonObj["dfb"] = BVAR[index];         // Variable name
    jsonObj["state"] = BVARval[index];    // Variable value
    return JSON.stringify(jsonObj);       // JSON object converted into a String.
  }
#endif

// ===============================================================================
// ANALOG OUTPUTS (PWM): Variables and functions
// ===============================================================================
#ifdef usePWM
  // Define the PWM output channels and ranges
  #define numPWMs 2
  // const array with PWM config. in format "{channel, rangeMin, rangeMax}"
  // array to store and report PWM values
  const int arrPWM[numPWMs][4] = {{5, 0, 1000}, {15, 50, 350}};
  int PWMval[numPWMs] = {0, 0};

  // Update analog feedback of PWM: return JSON object {"afb":5, "value":15}
  String updatePWM(byte index){
    JSONVar jsonObj;                      // Create JSON object for A.O. PWM's
    jsonObj["afb"] = arrPWM[index][0];    // Number of the PWM channel
    jsonObj["value"] = PWMval[index];     // converted value fo the A.O. in that channel
    return JSON.stringify(jsonObj);       // JSON object converted into a String.
  }
#endif

// ===============================================================================
// CONTROL ANALOG VARIABLES: Variables and functions
// ===============================================================================
#ifdef useAVAR
  // Define the PWM output channels and ranges
  #define numAVARS 2
  const char* AVAR[numAVARS] = {"tSET", "rhSET"};     // array with analog variable names 
  int AVARval[numAVARS] = {0, 0};                     // array to store analog variable values

  // Update analog feedback of control variable: return JSON object {"afb":"tSET", "value":22}
  String updateAVAR(byte index){
    JSONVar jsonObj;                      // Create JSON object for Analog Variables
    jsonObj["afb"] = AVAR[index];         // Analog Variable name
    jsonObj["value"] = AVARval[index];    // Analog Variable value
    return JSON.stringify(jsonObj);       // JSON object converted into a String.
  }
#endif
void updateOuts() {
  #ifdef useButton
    notifyClients(updateButton("STATE"));   // update Button field "state".
    notifyClients(updateButton("MODE"));    // update Button field "mode".
  #endif             
  #ifdef useToggle
    for (byte i:arrDO) { notifyClients(updateDO(i)); }
  #endif
  #ifdef usePWM
    for (byte i = 0; i < numPWMs; i++) { notifyClients(updatePWM(i)); }
  #endif
}
void updateVars() {
  #ifdef useBVAR
    for (byte i = 0; i < numBVARS; i++) { notifyClients(updateBVAR(i)); }
  #endif  
  #ifdef useAVAR
    for (byte i = 0; i < numAVARS; i++) { notifyClients(updateAVAR(i)); }
  #endif
}

// ===============================================================================
// MANAGE MESSAGES FROM CLIENTS (via WebSocket)
// ===============================================================================
  // Callback function to run when we receive new data from the clients via WebSocket protocol:
  // AwsFrameInfo provides information about the WebSocket frame being processed:
  // typedef struct { bool final; AwsFrameType opcode; bool isMasked; uint64_t payloadLength; uint8_t mask[4]; } AwsFrameInfo;
  // (AwsFrameInfo*)arg: It's converting the void* pointer arg to a pointer of type AwsFrameInfo*. This allows the function to access the WebSocket frame information stored in the arg parameter.
  void handleWSMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) { 
      data[len] = 0;
      // const char* msg = (char*)data;
      JSONVar jsonObj = JSON.parse((char*)data);

      //------------------------------------------------------
      // Refresh feedback for ALL BUTTONS & TOGGLE SWITCHES (requested by JS when the page is loaded):
      //------------------------------------------------------
      // JS function onOpen(event)  --> msg = `{"all": ""}`  --> Error: the last type is not received by JS. need to split in two separate steps
      if (jsonObj.hasOwnProperty("all")) {
        updateOuts();                         // Update feedback from all outputs when page loads.
        timer.setTimeout(500, updateVars);   // Update feedback from all variables when page loads. (timer to avoid error)
      }

      //------------------------------------------------------
      // Update BUTTON (operate D.O. in ESP and feedback to JS):
      //------------------------------------------------------
      // JS function press(element) --> msg {"but":"XXX"} (XXX is the button ID)
      #ifdef useButton
        else if (jsonObj.hasOwnProperty("but")) {
          const char* butName =  jsonObj["but"];
          if (strcmp(butName, "bON") == 0)  digitalWrite(statePin, 1);
          else if (strcmp(butName, "bOFF") == 0)  digitalWrite(statePin, 0);
          else if (strcmp(butName, "bAUTO") == 0)  digitalWrite(modePin, 1); 
          else if (strcmp(butName, "bMAN") == 0)   digitalWrite(modePin, 0);      
          notifyClients(butName+1);
        }
      #endif

      //------------------------------------------------------
      // Operate TOGGLE SWITCH (output in ESP and feedback to JS):
      //------------------------------------------------------
      // Digital output: JS function toggle(element) --> msg {"tog":"x"}
      #ifdef useToggle
        else if (jsonObj.hasOwnProperty("tog")) {
          const byte DOchannel = byte(jsonObj["tog"]);
          digitalWrite(DOchannel, !digitalRead(DOchannel));
          notifyClients(updateDO(DOchannel));
        }
      #endif

      // Boolean variable: JS function bvar(element) --> msg {"bvar":"x"}
      #ifdef useBVAR
        else if (jsonObj.hasOwnProperty("bvar")) {
          byte varIndex = 255;
          const char* varName = jsonObj["bvar"];
          for (byte i=0; i<numBVARS; i++) {
            if (strcmp(varName, BVAR[i]) == 0) { varIndex = i; break; }
          }
          if (varIndex == 255) return;
          BVARval[varIndex] = !BVARval[varIndex];
          notifyClients(updateBVAR(varIndex));
        }
      #endif  

      //------------------------------------------------------
      // Tune PWM A.O. (tune output in ESP and feedback to JS):
      //------------------------------------------------------
      // JS function tune(element, value) --> msg {"pwm":"x", "value":"xx"}
      #ifdef usePWM
        else if (jsonObj.hasOwnProperty("pwm")) {
          byte pwmIndex = 255;
          const byte pwmOutput = byte(jsonObj["pwm"]);
          for (byte i=0; i<numPWMs; i++) {
            if (pwmOutput == arrPWM[i][0]) { pwmIndex = i; break; }    // identify the output channel
          }
          if (pwmIndex == 255) return;
          PWMval[pwmIndex] = jsonObj["value"];  // update array PWMval with new value (keep 1 decimal place only)
          analogWrite(pwmOutput, map(PWMval[pwmIndex], arrPWM[pwmIndex][1], arrPWM[pwmIndex][2], 0, 255));  // Change (mapped) output signal.
          notifyClients(updatePWM(pwmIndex));         // Send feedback to JS.
        }
      #endif

      //------------------------------------------------------
      // Set CONTROL ANALOG VARIABLE
      //------------------------------------------------------
      // JS function avar(element, value) --> msg {"avar":"x", "value":"xx"}
      #ifdef useAVAR
        else if (jsonObj.hasOwnProperty("avar")) {
          byte varIndex = 255;
          const char* varName = jsonObj["avar"];
          for (byte i=0; i<numAVARS; i++) {
            if (strcmp(varName, AVAR[i]) == 0) { varIndex = i; break; }
          }
          if (varIndex == 255) return;
          AVARval[varIndex] = jsonObj["value"];
          notifyClients(updateAVAR(varIndex));
        }
      #endif
    }
  }

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, 
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
      client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWSMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);

  // Set GPIOs as outputs (buttons and switches)
  #ifdef useButton
    pinMode(statePin, OUTPUT);
    pinMode(modePin, OUTPUT);
    digitalWrite(statePin, 0);
    digitalWrite(modePin, 0);
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


  // Initialize wifi, file system and WebSocket: 
  initWiFi();
  initFS();
  initWebSocket();
  timer.setInterval(cleanTimer, clean);

  // Load index page when the server is called (on root "/")
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });

  // Serve files (JS, CSS and favicon) in a directory when requested by the root URL. 
  server.serveStatic("/", LittleFS, "/");
  server.begin();     // Start the server.
}

void loop() {
  timer.run();
}
