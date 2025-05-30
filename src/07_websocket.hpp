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
    JSONVar jsonObj = JSON.parse((char*)data);    // Parse the received JSON object
    //------------------------------------------------------
    // Elements in console.hpp
    //------------------------------------------------------
    if (jsonObj.hasOwnProperty("all"))          wsAll(jsonObj);
    #ifdef useButton
      else if (jsonObj.hasOwnProperty("but"))   wsButton(jsonObj);
    #endif
    #ifdef useToggle
      else if (jsonObj.hasOwnProperty("tog"))   wsToggle(jsonObj);
    #endif
    #ifdef useBVAR
      else if (jsonObj.hasOwnProperty("bvar"))  wsBvar(jsonObj);
    #endif  
    #ifdef usePWM
      else if (jsonObj.hasOwnProperty("pwm"))   wsPWM(jsonObj);
    #endif
    #ifdef useAVAR
      else if (jsonObj.hasOwnProperty("avar"))  wsAvar(jsonObj);
    #endif
  }
}

// ===============================================================================
// Handle events received via WebSocket; used in console and config.
// ===============================================================================
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, 
             void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    handleWSMessage(arg, data, len);
  }
  else if (Debug) {
    if (type == WS_EVT_CONNECT) {
      Serial.print(F("W.S. client connected: "));
      Serial.printf("#%u from %s\n", client->id(), client->remoteIP().toString().c_str());
    } 
    else if (type == WS_EVT_DISCONNECT) {
      Serial.print(F("W.S. client disconnected: "));
      Serial.printf("#%u\n", client->id());
    }
  }
}

// Function to initialize the websocket
void initWebSocket() {
  wsConsole.onEvent(onEvent);
  server.addHandler(&wsConsole);
  timer.setInterval(cleanTimer, [](){ wsConsole.cleanupClients(); });
  Serial.println(F("WebSocket initialized"));
}