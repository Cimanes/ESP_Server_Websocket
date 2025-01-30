#ifdef useConfig
  // =============================================
  // VARIABLES 
  // =============================================
  const byte inputLength = 5;    // Length of the input messages

  // Config. text inputs  (number, names, file paths and variable with values)
  #ifdef useTxIn                                      
    #define n_TxIn 2
    const char* TxName[n_TxIn] = {"t1", "t2"};
    const char* TxPath[n_TxIn] = {"t1.txt", "t2.txt"};
    const char* TxDefault[n_TxIn] = {"Y", "t2"};      // { BME period minutes, File size limit in KB }
    char TxVal[n_TxIn][inputLength];  // Initialize here
  #endif

  // Config. numeric inputs (number, names, file paths and variable with values)
  #ifdef useNumIn
    #define n_NumIn 2
    const char* NumName[n_NumIn] = {"n1", "n2"};          
    const char* NumPath[n_NumIn] = {"n1.txt", "n2.txt"};
    const char* NumDefault[n_NumIn] = {"15", "30"};      // { BME period minutes, File size limit in KB }
    char NumVal[n_NumIn][inputLength];
  #endif

  // Config. memory fields (number, names, file paths and variable with values)
  #ifdef useFbk
    #define n_Fbk 3
    const char* FbkName[n_Fbk] = {"f1", "f2", "f3"};          
    const char* FbkPath[n_Fbk] = {"f1.txt", "f2.txt", "f3.txt"};
    char FbkVal[n_Fbk][inputLength];
  #endif

  // =============================================
  // Prepare feedbackChar with current input values 
  // =============================================
  void getCurrentValues(){
    // memoryCheck();
    JSONVar values;
    #ifdef useTxIn                                      
      for (byte i = 0; i < n_TxIn; i++) { values[TxName[i]] = TxVal[i]; }
    #endif
    #ifdef useNumIn
      for (byte i = 0; i < n_NumIn; i++) { values[NumName[i]] = NumVal[i]; }
    #endif
    #ifdef useFbk
      values[FbkName[0]] = totalBytes;
      values[FbkName[1]] = fs_info.usedBytes/1000;
      values[FbkName[2]] = getFileSize(LittleFS, dataPath) * 100 / fileLimit;
    #endif
    JSON.stringify(values).toCharArray(feedbackChar, fbkLength);
    if (Debug == true) Serial.println(values);
  }

  // =============================================
  // Function to update BME interval  
  // =============================================
  void updateBMEinterval() {
    BMEperiod = 60000 * atoi(NumVal[0]);
    if(BMEtimer) timer.deleteTimer(BMEtimer);
    BMEtimer = timer.setInterval(BMEperiod, updateBME);
    if (Debug == true){
      Serial.print(F("timer: "));
      Serial.println(BMEperiod / 60000);
    }
  }

  void initConfig() {
    #ifdef useTxIn                                      
      for (byte i = 0; i < n_TxIn; i++) {   
        fileToCharPtr(LittleFS, TxPath[i], TxVal[i]);         // Initialize from file
        if(*TxVal[i] == '\0') strcpy(TxVal[i], TxDefault[i]);  // If file empty, initialize from default
        Serial.println(TxVal[i]);
      }
      if (TxVal[0][0] == 'N') Debug = false;
      else Debug = true; 
    #endif
    #ifdef useNumIn
      for (byte i = 0; i < n_NumIn; i++) { 
        fileToCharPtr(LittleFS, NumPath[i], NumVal[i]);           // Initialize from file
        if(*NumVal[i] == '\0') strcpy(NumVal[i], NumDefault[i]);  // If file empty, initialize from default
      }
    #endif
    #ifdef useBME
      BMEperiod = 60000 * atoi(NumVal[0]);
      fileLimit = 1000 * atoi(NumVal[1]);
    #endif

    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
      getCurrentValues();
      request->send(200, "application/json", feedbackChar);
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i = 0 ; i < params ; i++){
        const AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          for (byte i = 0; i < n_TxIn; i++) {          
            if (p->name() == TxName[i]) { 
              strcpy(TxVal[i], p->value().c_str());
              TxVal[i][sizeof(TxVal[i]) - 1] = '\0';
              if(i == 0) {
                if (TxVal[i][0] == 'Y')  Debug = true;
                else Debug = false;
              }
              if (Debug == true) {
                Serial.print(TxName[i]);
                Serial.print(F("set to: "));
                Serial.println(TxVal[i]);
                Serial.print(F("Debug: "));
                Serial.println(Debug);
              }
              writeFile(LittleFS, TxPath[i], TxVal[i]);
              break;
            }
          }
          for (byte i = 0; i < n_NumIn; i++) {          
            if (p->name() == NumName[i]) {
              strcpy(NumVal[i], p->value().c_str());
              NumVal[i][sizeof(NumVal[i]) - 1] = '\0';
              if (Debug == true) {
                Serial.print(NumName[i]);
                Serial.print(F("set to: "));
                Serial.println(NumVal[i]);
              }
              writeFile(LittleFS, NumPath[i], NumVal[i]);
              if(i == 0) updateBMEinterval();                     // BME timer interval is set to NumVal[0]
              else if(i == 1) fileLimit = 1000 * atoi(NumVal[1]); // Data-file limit is set to NumVal[1]
              break;
            }
          }         
        }
      }
      request->send(LittleFS, "/config.html", "text/html");
    });
  }
#endif