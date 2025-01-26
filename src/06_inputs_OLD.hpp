#include "05_bme.hpp"
#ifdef useConfig
  // =============================================
  // VARIABLES 
  // =============================================
  // Search for parameter in HTTP POST request
  const char* PARAM_INPUT_1 = "in_1";
  const char* PARAM_INPUT_2 = "in_2";

  //Variables to save values from HTML form
  const byte inputLength = 10;
  char in_1[inputLength];
  char in_2[inputLength];

  // File paths to save input values permanently
  const char* in_1Path = "/in_1.txt";
  const char* in_2Path = "/in_2.txt";

  void getCurrentValues(){
    JSONVar values;
    values["in_1"] = in_1;
    values["in_2"] = in_2;
    JSON.stringify(values).toCharArray(feedbackChar, fbkLength);
  }

  void initConfig() {
    // Load values saved in SPIFFS
    fileToCharPtr(LittleFS, in_1Path, in_1);
    fileToCharPtr(LittleFS, in_2Path, in_2);
    
    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
      getCurrentValues();
      request->send(200, "application/json", feedbackChar);
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST in_1 value
          if (p->name() == PARAM_INPUT_1) {
            strcpy(in_1, p->value().c_str());
            in_1[sizeof(in_1) - 1] = '\0';
            Serial.print("Input 1 set to: ");
            Serial.println(in_1);
            writeFile(LittleFS, in_1Path, in_1);
          }
          // HTTP POST in_2 value
          if (p->name() == PARAM_INPUT_2) {
            strcpy(in_2, p->value().c_str());
            in_2[sizeof(in_2) - 1] = '\0';
            Serial.print("Input 2 set to: ");
            Serial.println(in_2);
            writeFile(LittleFS, in_2Path, in_2);
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(LittleFS, "/config.html", "text/html");
    });
  }
#endif