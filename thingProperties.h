#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char DEVICE_LOGIN_NAME[]  = "e8e050d2-83b3-4a24-8ab5-4529d27496a7";
const char SSID[]               = SECRET_SSID;    
const char PASS[]               = SECRET_OPTIONAL_PASS;    
const char DEVICE_KEY[]         = SECRET_DEVICE_KEY;    

void onResetStatsChange();
void onTriggerRoundChange();
void onAutoModeChange(); 

String gameStatus;
int avgTime, bestTime, reflexTime, cloudRoundCount;
bool resetStats, triggerRound, autoMode, falseStart;

void initProperties(){
  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(gameStatus, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(avgTime, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(bestTime, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(reflexTime, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(cloudRoundCount, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(falseStart, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(resetStats, READWRITE, ON_CHANGE, onResetStatsChange);
  ArduinoCloud.addProperty(triggerRound, READWRITE, ON_CHANGE, onTriggerRoundChange);
  ArduinoCloud.addProperty(autoMode, READWRITE, ON_CHANGE, onAutoModeChange);
}

WiFiConnectionHandler Arduino_ConnectionHandler(SSID, PASS);