#include "config.hpp"

/***************************************************
****************************************************
1) CONSOLE (WEBSOCKET)
****************************************************
***************************************************/

// ===============================================================================
// BUTTONS: update "feedback" to send to customer
// ===============================================================================
#ifdef useButton
  // function "updateButton()": Replace placeholders found in HTML (%STATE%, %MODE%...) with their current value

void updateButton(const char var[]) {
    if (strcmp(var, "STATE") == 0) {
        strcpy(feedbackChar, digitalRead(statePin) ? "ON" : "OFF");
    }
    else if (strcmp(var, "MODE") == 0) {
        strcpy(feedbackChar, digitalRead(modePin) ? "AUTO" : "MAN");
    }
    else feedbackChar[0] = '\0';    
    ws.textAll(feedbackChar);
}
#endif

// ===============================================================================
// TOGGLE SWITCHES (Digital outputs): update "feedback" to send to customer
// ===============================================================================
#ifdef useToggle
  // Update toggle switch: return JSON object {"dfb":12, "state":1}
  // const char* updateDO(byte gpio){   // const char* function and return pointer
    // JSONVar jsonObj;
    // jsonObj["dfb"] = gpio;                  // Number of the GPIO
    // jsonObj["state"] = digitalRead(gpio);   // 0 or 1
    // JSON.stringify(jsonObj).toCharArray(feedbackChar, 50);
    // return feedbackChar;                    // JSON object converted into a String.
  // }    
    void updateDO(byte gpio){               // void function and send feedback
    JSONVar jsonObj;
    jsonObj["dfb"] = gpio;                  // Number of the GPIO
    jsonObj["state"] = digitalRead(gpio);   // 0 or 1
    JSON.stringify(jsonObj).toCharArray(feedbackChar, 50);
    ws.textAll(feedbackChar);
  }
#endif

// ===============================================================================
// CONTROL BOOLEAN VARIABLES: update "feedback" to send to customer
// ===============================================================================
#ifdef useBVAR
  // Update boolean feedback of control variable: return JSON object {"dfb":"bVAR1", "state":0}
  void updateBVAR(byte index){
    JSONVar jsonObj;                      // Create JSON object for boolean Variables
    jsonObj["dfb"] = BVAR[index];         // Variable name
    jsonObj["state"] = BVARval[index];    // Variable value
    JSON.stringify(jsonObj).toCharArray(feedbackChar, 50);
    ws.textAll(feedbackChar);
  }
#endif

// ===============================================================================
// ANALOG OUTPUTS (PWM): update "feedback" to send to customer
// ===============================================================================
#ifdef usePWM
  // Update analog feedback of PWM: return JSON object {"afb":5, "value":15}
  void updatePWM(byte index){
    JSONVar jsonObj;                      // Create JSON object for A.O. PWM's
    jsonObj["afb"] = arrPWM[index][0];    // Number of the PWM channel
    jsonObj["value"] = PWMval[index];     // converted value fo the A.O. in that channel
    JSON.stringify(jsonObj).toCharArray(feedbackChar, 50);
    ws.textAll(feedbackChar);
  }
#endif

// ===============================================================================
// CONTROL ANALOG VARIABLES: update "feedback" to send to customer
// ===============================================================================
#ifdef useAVAR
  // Update analog feedback of control variable: return JSON object {"afb":"tSET", "value":22}
  void updateAVAR(byte index){
    JSONVar jsonObj;                      // Create JSON object for Analog Variables
    jsonObj["afb"] = AVAR[index];         // Analog Variable name
    jsonObj["value"] = AVARval[index];    // Analog Variable value
    JSON.stringify(jsonObj).toCharArray(feedbackChar, 50);
    ws.textAll(feedbackChar);
  }
#endif

// ===============================================================================
// FUNCTIONS TO UPDATE VALUES OF VARIABLES AND OUTPUTS
// ===============================================================================
// Function to notify all clients with a message (JSON object)
// void notifyClients(const char* msg) { ws.textAll(msg); }

void updateOuts() {
  #ifdef useButton
    updateButton("STATE");   // update Button field "state".
    updateButton("MODE");    // update Button field "mode".
  #endif             
  #ifdef useToggle
    // for (byte i:arrDO) { notifyClients(updateDO(i)); } // --> using "const char* function" updateDO
    for (byte i:arrDO) { updateDO(i); }                   // using "void function" updateDO
  #endif
  #ifdef usePWM
    for (byte i = 0; i < numPWMs; i++) { updatePWM(i); }
  #endif
}
void updateVars() {
  #ifdef useBVAR
    for (byte i = 0; i < numBVARS; i++) { updateBVAR(i); }
  #endif  
  #ifdef useAVAR
    for (byte i = 0; i < numAVARS; i++) { updateAVAR(i); }
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
    JSONVar jsonObj = JSON.parse((char*)data);

    //------------------------------------------------------
    // Refresh feedback for ALL outputs and variables (requested by JS when the page is loaded):
    //------------------------------------------------------
    // JS function onOpen(event)  --> msg = `{"all": ""}`  --> Error: the last type is not received by JS. need to split in two separate steps
    if (jsonObj.hasOwnProperty("all")) {
      updateOuts();                         // Update feedback from all outputs when page loads.
      timer.setTimeout(500, updateVars);    // Update feedback from all variables when page loads. (timer to avoid error)
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
        ws.textAll(butName+1);
      }
    #endif

    /*-------------------------------------------------------
    Operate TOGGLE SWITCH (output in ESP and feedback to JS):
    --------------------------------------------------------*/
    // Digital output: JS function toggle(element) --> msg {"tog":"x"}
    #ifdef useToggle
      else if (jsonObj.hasOwnProperty("tog")) {
        const byte DOchannel = byte(jsonObj["tog"]);
        digitalWrite(DOchannel, !digitalRead(DOchannel));
        // notifyClients(updateDO(DOchannel));
        updateDO(DOchannel);
      }
    #endif

    /*-------------------------------------------------------
    Set BOOLEAN VARIABLE (state in ESP and feedback to JS)
    --------------------------------------------------------*/
    // JS function bvar(element) --> msg {"bvar":"x"}
    #ifdef useBVAR
      else if (jsonObj.hasOwnProperty("bvar")) {
        byte varIndex = 255;
        const char* varName = jsonObj["bvar"];
        for (byte i=0; i<numBVARS; i++) {
          if (strcmp(varName, BVAR[i]) == 0) { varIndex = i; break; }
        }
        if (varIndex == 255) return;
        BVARval[varIndex] = !BVARval[varIndex];
        updateBVAR(varIndex);
      }
    #endif  

    /*-------------------------------------------------------
    Tune PWM A.O. (tune output in ESP and feedback to JS):
    --------------------------------------------------------*/
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
        updatePWM(pwmIndex);         // Send feedback to JS.
      }
    #endif

    /*-------------------------------------------------------
    Set ANALOG VARIABLE (value in ESP and feedback to JS)
    --------------------------------------------------------*/
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
        updateAVAR(varIndex);
      }
    #endif
  }
}

// ===============================================================================
// Handle events received via WebSocket:
// ===============================================================================
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

// Callback function to periodically clean websocket clients:
void clean() { ws.cleanupClients();}

// Function to initialize the websocket
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


/***************************************************
****************************************************
2) BME280 (SSE: realtime update or event stream )
****************************************************
****************************************************/
#ifdef useBME
  // Callback function to periodically retrieve BME data:
  // Update object BMEread. return JSON object {"t":5, "rh":15, "p":1023}
  void readBME(){
    JSONVar jsonObj;                                      // Create JSON object for bme readings
    jsonObj["t"]  = int(bme.readTemperature() * aFactor); // Temperature key-value (ºC float 1 decimal to int)
    jsonObj["rh"] = int(bme.readHumidity() * aFactor);    // Humidity key-value (% float 1 decimal to int)
    jsonObj["p"]  = int(bme.readPressure() / 10);         // Pressure key-value (Pascals to DPascals)
    JSON.stringify(jsonObj).toCharArray(feedbackChar, 50);// Return JSON object as char array.
  }

  // Periodic update of BME values --> send event "new_readings"
  void updateBME() {
    readBME();
    events.send("ping", NULL, millis());
    events.send(feedbackChar, "new_readings", millis());
  }
#endif

void setup() {
  Serial.begin(115200);

  // Set GPIO modes and initial values
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

  // Initialize wifi, file system, WebSocket and setup timer: 
  #ifdef useBME
    initBME();
    timer.setInterval(BMETimer, updateBME);
  #endif  
  initWiFi();
  initFS();
  initWebSocket();
  timer.setInterval(cleanTimer, clean);

  // Load index page when the server is called (on root "/")
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });

  #ifdef useBME
    // Request received for latest sensor readings  --> Response HTTP to request received "/readings"
    server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request) {
      readBME();
      request->send(200, "application/json", feedbackChar);
    });

    events.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        Serial.printf("Client reconnected! Last message ID: %u\n", client->lastId());
      }
      // send event with message "hello!", id current millis
      // and set reconnect delay to 1 second
      client->send("hello!", NULL, millis(), 1000);
    });
    server.addHandler(&events);
  #endif
  
  // Serve files (JS, CSS and favicon) in a directory when requested by the root URL. 
  server.serveStatic("/", LittleFS, "/");
  server.begin();     // Start the server.
}

void loop() {
  timer.run();
}
