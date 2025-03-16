// =============================================
// PIN DEFINITIONS 
// =============================================
#if defined(ESP32)
  #ifdef useButton                       // Config. D.O. GPIO's used by buttons (ON/OFF, AUTO/MAN)
    #define statePin 32
    #define modePin 33
  #endif
  #ifdef useToggle                       // Config. toggle switches
    #define n_DOs 2                     // Number of toggle switches
    const byte arrDO[n_DOs] = {34, 35}; // D.O. GPIO's used by toggle switches
  #endif
  #ifdef usePWM                          // Config. PWM analog outputs
    #define n_PWMs 2                    // Number of analog outputs
    // Config PWM. Format "{channel, rangeMin, rangeMax}"
    const int arrPWM[n_PWMs][4] = {{16, 0, 1000}, {17, 50, 350}};
    int PWMval[n_PWMs] = {0, 0};   // Store and report PWM values
  #endif
#elif defined(ESP8266)
  #ifdef useButton                      // Config. D.O. GPIO's used by buttons (ON/OFF, AUTO/MAN)
    #define statePin 0
    #define modePin 2
  #endif
  #ifdef useToggle                      // Config. toggle switches
    #define n_DOs 2                     // Number of toggle switches
    const byte arrDO[n_DOs] = {12, 14}; // D.O. GPIO's used by toggle switches
  #endif
  #ifdef usePWM                         // Config. PWM analog outputs
    #define n_PWMs 2                    // Number of analog outputs
    // Config PWM. Format "{channel, rangeMin, rangeMax}" (match ranges with JS and )
    const int arrPWM[n_PWMs][4] = {{13, 0, 1000}, {15, 0, 2000}};
    int PWMval[n_PWMs] = {0, 0};   // Store and report PWM values
  #endif
#endif

// =============================================
// GLOBAL VARIABLES 
// =============================================
// Config. boolean variables
#ifdef useBVAR                                      
  #define n_BVARS 2                                // Number of boolean variables
  const char* BVAR[n_BVARS] = {"bVAR1", "bVAR2"};  // Boolean variable names 
  bool BVARval[n_BVARS] = {0, 0};                  // Store boolean variable initial values
#endif

// Config. analog variables (match initial value with JS range)
#ifdef useAVAR                                        
  #define n_AVARS 2                                // Number of analog variables
  const char* AVAR[n_AVARS] = {"tSET", "rhSET"};   // Analog variable names 
  int AVARval[n_AVARS] = {50, 0};                  // Analog variable initial values  
#endif


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
    wsConsole.textAll(feedbackChar);
  }

  void initButton() {
    pinMode(statePin, OUTPUT);
    pinMode(modePin, OUTPUT);
    digitalWrite(statePin, 0);
    digitalWrite(modePin, 1);    
  }
#endif

// ===============================================================================
// TOGGLE SWITCHES (Digital outputs): update "feedback" to send to customer
// ===============================================================================
#ifdef useToggle
  // Update toggle switch: return JSON object {"dfb":12, "state":1}
  void updateDO(byte gpio){                 // Use void function and send feedback
    JSONVar jsonObj;
    jsonObj["dfb"] = gpio;                  // Number of the GPIO
    jsonObj["state"] = digitalRead(gpio);   // 0 or 1
    JSON.stringify(jsonObj).toCharArray(feedbackChar, fbkLength);
    wsConsole.textAll(feedbackChar);
  }

  void initToggle() {
    for (byte i =0; i<n_DOs; i++) { pinMode(arrDO[i], OUTPUT); }
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
    JSON.stringify(jsonObj).toCharArray(feedbackChar, fbkLength);
    wsConsole.textAll(feedbackChar);
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
    JSON.stringify(jsonObj).toCharArray(feedbackChar, fbkLength);
    wsConsole.textAll(feedbackChar);
  }

  void  initPWM() {
    for (byte i =0; i<n_PWMs; i++) { 
      pinMode(arrPWM[i][0], OUTPUT);
      analogWrite(arrPWM[i][0], 0);
    }
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
    JSON.stringify(jsonObj).toCharArray(feedbackChar, fbkLength);
    wsConsole.textAll(feedbackChar);
  }
#endif

// ===============================================================================
// FUNCTIONS TO UPDATE VALUES OF VARIABLES AND OUTPUTS
// ===============================================================================
// Functions to notify all clients with a message (JSON object)
void updateOuts() {
  #ifdef useButton
    updateButton("STATE");   // update Button field "state".
    updateButton("MODE");    // update Button field "mode".
  #endif             
  #ifdef useToggle
    for (byte i:arrDO) { updateDO(i); }                   // using "void function" updateDO
  #endif
  #ifdef usePWM
    for (byte i = 0; i < n_PWMs; i++) { updatePWM(i); }
  #endif
}
void updateVars() {
  #ifdef useBVAR
    for (byte i = 0; i < n_BVARS; i++) { updateBVAR(i); }
  #endif  
  #ifdef useAVAR
    for (byte i = 0; i < n_AVARS; i++) { updateAVAR(i); }
  #endif
}

// ===============================================================================
//  FUNCTIONS TO HANDLE MESSAGES FROM CLIENTS (via WebSocket) 
// ===============================================================================

// Refresh feedback for ALL outputs and variables (requested by JS when the page is loaded):
// JS function onOpen(event)  --> msg = `{"all": ""}`  --> Error: the last type is not received by JS. need to split in two separate steps
void wsAll(JSONVar jsonObj) {
  updateOuts();
  timer.setTimeout(500, updateVars);    // Update feedback from all variables when page loads. (timer to avoid error)
}

// Update BUTTON (operate D.O. in ESP and feedback to JS):
// JS function press(element) --> msg {"but":"XXX"} (XXX is the button ID)
void wsButton(JSONVar jsonObj) {
  const char* butName =  jsonObj["but"];
  if (strcmp(butName, "bON") == 0)  digitalWrite(statePin, 1);
  else if (strcmp(butName, "bOFF") == 0)  digitalWrite(statePin, 0);
  else if (strcmp(butName, "bAUTO") == 0)  digitalWrite(modePin, 1); 
  else if (strcmp(butName, "bMAN") == 0)   digitalWrite(modePin, 0);      
  wsConsole.textAll(butName+1);
}

// Update TOGGLE SWITCH (operate D.O. in ESP and feedback to JS):
// Digital output: JS function toggle(element) --> msg {"tog":"x"}
void wsToggle(JSONVar jsonObj) {
  const byte DOchannel = byte(jsonObj["tog"]);
  digitalWrite(DOchannel, !digitalRead(DOchannel));
  updateDO(DOchannel);
}

// Update BOOLEAN VARIABLE (state in ESP and feedback to JS)
// JS function bvar(element) --> msg {"bvar":"x"}
void wsBvar(JSONVar jsonObj) {
  byte varIndex = 255;
  const char* varName = jsonObj["bvar"];
  for (byte i=0; i<n_BVARS; i++) {
    if (strcmp(varName, BVAR[i]) == 0) { varIndex = i; break; }
  }
  if (varIndex == 255) return;
  BVARval[varIndex] = !BVARval[varIndex];
  updateBVAR(varIndex);
}

// Update PWM (tune output in ESP and feedback to JS):
// JS function tune(element, value) --> msg {"pwm":"x", "value":"xx"}
void wsPWM(JSONVar jsonObj) {
  byte pwmIndex = 255;
  const byte pwmOutput = byte(jsonObj["pwm"]);
  for (byte i=0; i<n_PWMs; i++) {
    if (pwmOutput == arrPWM[i][0]) { pwmIndex = i; break; }    // identify the output channel
  }
  if (pwmIndex == 255) return;
  PWMval[pwmIndex] = jsonObj["value"];  // update array PWMval with new value (keep 1 decimal place only)
  analogWrite(pwmOutput, map(PWMval[pwmIndex], arrPWM[pwmIndex][1], arrPWM[pwmIndex][2], 0, 255));  // Change (mapped) output signal.
  updatePWM(pwmIndex);         // Send feedback to JS.
}

// Update ANALOG VARIABLE (value in ESP and feedback to JS)
// JS function avar(element, value) --> msg {"avar":"x", "value":"xx"}
void wsAvar(JSONVar jsonObj) {
  byte varIndex = 255;
  const char* varName = jsonObj["avar"];
  for (byte i=0; i<n_AVARS; i++) {
    if (strcmp(varName, AVAR[i]) == 0) { varIndex = i; break; }
  }
  if (varIndex == 255) return;
  AVARval[varIndex] = (int)jsonObj["value"];
  updateAVAR(varIndex);
}

