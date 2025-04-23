//Android IOT Cloud Strings: Fill in accordingly
#define SECRET_DEVICE_KEY ""
#define SECRET_OPTIONAL_PASS ""
#define SECRET_SSID ""
#define SECRET_OPTIONAL_PASS2 ""
#define SECRET_SSID2 ""
const char DEVICE_LOGIN_NAME[]  = "";



#include <ESP8266WiFi.h>
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char SSID2[]               = SECRET_SSID2;    // Network SSID (name)
const char PASS2[]               = SECRET_OPTIONAL_PASS2;
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password

void onAlertChange();
void onBkltChange();
void onTracfetchChange();
String alert="No new alerts";
CloudDimmedLight bklt;


float arh;
float atemp;
bool tracfetch;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(alert, READWRITE, ON_CHANGE, onAlertChange);
  ArduinoCloud.addProperty(bklt, READWRITE, ON_CHANGE, onBkltChange);
  ArduinoCloud.addProperty(arh, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(atemp, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(tracfetch, READWRITE, ON_CHANGE, onTracfetchChange);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

