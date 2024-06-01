#include "05_bme.hpp"
#ifdef useInputs
  // =============================================
  // VARIABLES 
  // =============================================
  // Search for parameter in HTTP POST request
  const char* PARAM_INPUT_1 = "in_1";
  const char* PARAM_INPUT_2 = "in_2";

  //Variables to save values from HTML form
  String in_1;
  String in_2;

  // File paths to save input values permanently
  const char* in_1Path = "/in_1.txt";
  const char* in_2Path = "/in_2.txt";

  JSONVar values;
  String getCurrentInputValues(){
    values["in_1"] = in_1;
    values["in_2"] = in_2;
    String jsonString = JSON.stringify(values);
    return jsonString;
  }

  void initInputs() {
    // Load values saved in SPIFFS
    in_1 = readFile(LittleFS, in_1Path);
    in_2 = readFile(LittleFS, in_2Path);

    server.on("/inputs", HTTP_GET, [](AsyncWebServerRequest *request){
      String json = getCurrentInputValues();
      request->send(200, "application/json", json);
      json = String();
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST in_1 value
          if (p->name() == PARAM_INPUT_1) {
            in_1 = p->value().c_str();
            Serial.print("Input 1 set to: ");
            Serial.println(in_1);
            // Write file to save value
            writeFile(LittleFS, in_1Path, in_1.c_str());
          }
          // HTTP POST in_2 value
          if (p->name() == PARAM_INPUT_2) {
            in_2 = p->value().c_str();
            Serial.print("Input 2 set to: ");
            Serial.println(in_2);
            // Write file to save value
            writeFile(LittleFS, in_2Path, in_2.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(LittleFS, "/inputs.html", "text/html");
    });
  }
#endif
