#include "05_bme.hpp"

#ifdef useInputs
  // =============================================
  // VARIABLES 
  // =============================================
  const byte inputLength = 10;    // Length of the input messages

  // Config. text inputs  (number, names, file paths and variable with values)
  #ifdef useTxIn                                      
    #define n_TxIn 2
    const char* TxName[n_TxIn] = {"tx_1", "tx_2"};
    const char* TxPath[n_TxIn] = {"tx_1.txt", "tx_2.txt"};
    char TxVal[n_TxIn][inputLength] = {"Text_1000", "Text_2000"};  // Initialize here
  #endif

  // Config. numeric inputs (number, names, file paths and variable with values)
  #ifdef useNumIn
    #define n_NumIn 2
    const char* NumName[n_NumIn] = {"num_1", "num_2"};
    const char* NumPath[n_NumIn] = {"num_1.txt", "num_2.txt"};
    char NumVal[n_NumIn][inputLength];
  #endif

  // =============================================
  // Prepare feedbackChar with current input values 
  // =============================================
  void getCurrentInputValues(){
    JSONVar values;
    for (byte i = 0; i < n_TxIn; i++) { values[TxName[i]] = TxVal[i]; }
    for (byte i = 0; i < n_NumIn; i++) { values[NumName[i]] = NumVal[i]; }
    JSON.stringify(values).toCharArray(feedbackChar, fbkLength);
  }

  void initInputs() {
    // Update initial values
    ultoa(BMEperiod, NumVal[0], 10);  
    strcpy(NumVal[1], "100");

    for (byte i = 0; i < n_TxIn; i++) { fileToCharPtr(LittleFS, TxPath[i], TxVal[i]); }
    for (byte i = 0; i < n_NumIn; i++) { fileToCharPtr(LittleFS, NumPath[i], NumVal[i]); }
    Serial.println(NumVal[0]);

    server.on("/inputs", HTTP_GET, [](AsyncWebServerRequest *request){
      getCurrentInputValues();
      request->send(200, "application/json", feedbackChar);
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          for (byte i = 0; i < n_TxIn; i++) {          
            if (p->name() == TxName[i]) { 
              strcpy(TxVal[i], p->value().c_str());
              TxVal[i][sizeof(TxVal[i]) - 1] = '\0';
              Serial.print(TxName[i]);
              Serial.print("set to: ");
              Serial.println(TxVal[i]);
              writeFile(LittleFS, TxPath[i], TxVal[i]);
              break;
            }
          }
          for (byte i = 0; i < n_NumIn; i++) {          
            if (p->name() == NumName[i]) { 
              strcpy(NumVal[i], p->value().c_str());
              NumVal[i][sizeof(NumVal[i]) - 1] = '\0';
              Serial.print(NumName[i]);
              Serial.print("set to: ");
              Serial.println(NumVal[i]);
              writeFile(LittleFS, NumPath[i], NumVal[i]);
              break;
            }
          }
        }
      }
      request->send(LittleFS, "/inputs.html", "text/html");
    });
  }
#endif