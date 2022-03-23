#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>

/***********************************
// ENVIRONMENTS
***********************************/
#define LED_PIN 2
#define DEFAULT_AP_SSID "smartholic"
#define DEFAULT_AP_PASSWD "12345678"

#define OFFSET_OF_SERIAL_BAUD_RATE 0
#define OFFSET_OF_AP 4

#define MIN_BAUD_RATE_OF_SERIAL 300
#define MAX_BAUD_RATE_OF_SERIAL 115200

#define PIN_0 0
#define PIN_1 2

ESP8266WebServer server(80);


/***********************************
// Serial Settings
***********************************/
int getSerialBaudRate() {
  int baudRate = 0;
  EEPROM.begin(128);
  EEPROM.get(OFFSET_OF_SERIAL_BAUD_RATE, baudRate);
  EEPROM.end();
  return getSaftySerialBaudRate(baudRate);
}


int setSerialBaudRate(int baudRate) {
  baudRate = getSaftySerialBaudRate(baudRate);
  EEPROM.begin(128);
  EEPROM.put(OFFSET_OF_SERIAL_BAUD_RATE, baudRate);
  EEPROM.commit();
  EEPROM.end();
  return baudRate;
}


int getSaftySerialBaudRate(int baudRate) {
  return (baudRate >= MIN_BAUD_RATE_OF_SERIAL && baudRate <= MAX_BAUD_RATE_OF_SERIAL) ? baudRate : MAX_BAUD_RATE_OF_SERIAL;
}



/***********************************
// AP Settings
***********************************/
struct Ap {
  char ssid[32+1];
  char passwd[16+1];
};

void getAp(struct Ap* ap) {
  EEPROM.begin(128);
  EEPROM.get(OFFSET_OF_AP, *ap);
  EEPROM.end();
  if(strlen(ap->ssid) <= 0) {
    strcpy(ap->ssid, DEFAULT_AP_SSID);
    strcpy(ap->passwd, DEFAULT_AP_PASSWD);
  }
}


void setAp(struct Ap ap) {
  EEPROM.begin(128);
  EEPROM.put(OFFSET_OF_AP, ap);
  EEPROM.commit();
  EEPROM.end();
}



/***********************************
// Server
***********************************/
void startServer() {
  server.on("/", HTTP_GET, handleGetIndex);
  server.on("/network/ap", HTTP_GET, handleGetNetworkAP);
  server.on("/network/ap", HTTP_POST, handlePostNeworkAP);
  server.on("/network/internet", HTTP_GET, handleGetNetworkInternet);
  server.on("/network/internet", HTTP_POST, handlePostNeworkInternet);
  server.on("/pins/0", HTTP_GET, handleGetPin0);
  server.on("/pins/0", HTTP_PUT, handlePutPin0);
  server.on("/pins/1", HTTP_GET, handleGetPin1);
  server.on("/pins/1", HTTP_PUT, handlePutPin1);
  server.on("/serial", HTTP_POST, handlePostSerial);
  server.on("/serial/setup", HTTP_GET, handleGetSerialSetup);
  server.on("/serial/setup", HTTP_POST, handlePostSerialSetup);
  server.onNotFound(handleNotFound);
  server.begin();
  
  WiFi.begin();
}



void startMDNS() {
  MDNS.begin("smart");
}



void handleGetIndex() {
  struct Ap ap = { "", "" }; getAp(&ap);
  String resOfAp = "{ \"status\": \""+ getWifiStatus() +"\", \"ssid\": \""+ ap.ssid +"\", \"passwd\": \"" + ap.passwd + "\", \"ip\": \""+ WiFi.softAPIP().toString() +"\" }";
  String resOfInternet= "{ \"status\": \""+ getWifiStatus() +"\", \"ap\": \""+ WiFi.SSID() +"\", \"ip\": \""+ WiFi.localIP().toString() +"\" }";
  
  int pin0Status = digitalRead(PIN_0);
  String resOfPin0 = "{ \"status\": \"";
  resOfPin0.concat(pin0Status == 1 ? "on" : "off");
  resOfPin0.concat("\" }");

  int pin1Status = digitalRead(PIN_0);
  String resOfPin1 = "{ \"status\": \"";
  resOfPin1.concat(pin1Status == 1 ? "on" : "off");
  resOfPin1.concat("\" }");

  int baudRate = getSerialBaudRate();
  String resOfSerial = "{ \"baudRate\": ";
  resOfSerial.concat(baudRate);
  resOfSerial.concat(" }");
  
  server.send(200, "application/json", "{ \"network\": { \"ap\":" + resOfAp + ", \"internet\": "+ resOfInternet +" }, \"pin0\": "+ resOfPin0 +", \"pin1\": "+ resOfPin1 +", \"serial\": "+ resOfSerial +" }");
}



String getWifiStatus() {
  switch(WiFi.status()) {
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "UNKNOWN";
  }
}



void handleGetNetworkAP() {
  struct Ap ap = { "", "" }; getAp(&ap);
  server.send(200, "application/json", "{ \"status\": \""+ getWifiStatus() +"\", \"ssid\": \""+ ap.ssid +"\", \"passwd\": \"" + ap.passwd + "\", \"ip\": \""+ WiFi.softAPIP().toString() +"\" }");
}



void handlePostNeworkAP() {
  DynamicJsonDocument* payload = getPayload();
  if(payload == NULL) { return; }

  if(!payload->containsKey("ssid")) { 
    server.send(400, "application/json", "{ \"error\": \"Supply 'ssid'.\" }");
    payload->clear(); delete payload;
    return; 
  }
  
  String ssid = (*payload)["ssid"].as<String>();
  String passwd = (*payload)["passwd"].as<String>();

  if(ssid.length() < 1 || ssid.length() > 32) {
    server.send(400, "application/json", "{ \"error\": \"'ssid' should be between 1 ~ 32.\" }");
    payload->clear(); delete payload;
    return;
  }

  if(passwd.length() > 0 && (passwd.length() < 8 || passwd.length() > 16)) {
    server.send(400, "application/json", "{ \"error\": \"'passwd' should be between 8 ~ 16.\" }");
    payload->clear(); delete payload;
    return;
  }

  server.send(200, "application/json", "{ }");
  server.client().flush();

  // save ap
  struct Ap ap;
  strcpy(ap.ssid, ssid.c_str());
  strcpy(ap.passwd, passwd.c_str());
  setAp(ap);
  
  WiFi.softAP(ssid, passwd);
  payload->clear();
  delete payload;
}



void handleGetNetworkInternet() {
  server.send(200, "application/json", "{ \"status\": \""+ getWifiStatus() +"\", \"ap\": \""+ WiFi.SSID() +"\", \"ip\": \""+ WiFi.localIP().toString() +"\" }");
}



void handlePostNeworkInternet() {
  DynamicJsonDocument* payload = getPayload();
  if(payload == NULL) { return; }

  if(!payload->containsKey("ssid")) { 
    server.send(400, "application/json", "{ \"error\": \"Supply 'ssid'.\" }");
    payload->clear(); delete payload;
    return; 
  }
  
  String ssid = (*payload)["ssid"].as<String>();
  String passwd = (*payload)["passwd"].as<String>();

  server.send(200, "application/json", "{ }");
  server.client().flush();
  
  WiFi.begin(ssid, passwd);
  payload->clear();
  delete payload;
}



void handleGetPin0() {
  int pinStatus = digitalRead(PIN_0);
  String response = "{ \"status\": \"";
  response.concat(pinStatus == 1 ? "on" : "off");
  response.concat("\" }");
  server.send(200, "application/json", response);
}



void handlePutPin0() {
  DynamicJsonDocument* payload = getPayload();
  if(payload == NULL) { return; }

  if(!payload->containsKey("status")) { 
    server.send(400, "application/json", "{ \"error\": \"Supply 'status'.\" }");
    payload->clear(); delete payload;
    return; 
  }
  
  bool turnOn = ((*payload)["status"].as<String>() == "on");

  digitalWrite(PIN_0, turnOn ? HIGH : LOW);
  String response = "{ \"status\": \"";
  response.concat(turnOn ? "on" : "off");
  response.concat("\" }");
  server.send(200, "application/json", response);
  payload->clear();
  delete payload;
}



void handleGetPin1() {
  int pinStatus = digitalRead(PIN_1);
  String response = "{ \"status\": \"";
  response.concat(pinStatus == 1 ? "on" : "off");
  response.concat("\" }");
  server.send(200, "application/json", response);
}



void handlePutPin1() {
  DynamicJsonDocument* payload = getPayload();
  if(payload == NULL) { return; }
  if(!payload->containsKey("status")) { 
    server.send(400, "application/json", "{ \"error\": \"Supply 'status'.\" }");
    payload->clear(); delete payload;
    return; 
  }
  
  bool turnOn = ((*payload)["status"].as<String>() == "on");

  digitalWrite(PIN_1, turnOn ? HIGH : LOW);
  String response = "{ \"status\": \"";
  response.concat(turnOn ? "on" : "off");
  response.concat("\" }");
  server.send(200, "application/json", response);
  payload->clear();
  delete payload;
}



void handlePostSerial() {
  if(server.hasArg("plain") == false) {
    server.send(400, "application/json", "{ \"error\": \"Data not found.\" }");
    return;
  }

  String data = server.arg("plain");
  // clear stream buffer
  while(Serial.available()) {
    Serial.read();
  }
  Serial.println(data.c_str());
  String response = Serial.readStringUntil('\n');
  server.send(200, "text/plain", response);
}



void handleGetSerialSetup() {
  String response = "{ \"baudRate\": ";
  response.concat(getSerialBaudRate());
  response.concat(" }");
  server.send(200, "application/json", response);
}



void handlePostSerialSetup() {
  DynamicJsonDocument* payload = getPayload();
  if(payload == NULL) { return; }

  if(!payload->containsKey("baudRate")) { 
    server.send(400, "application/json", "{ \"error\": \"Supply 'baudRate'.\" }");
    payload->clear(); delete payload;
    return; 
  }
  
  int baudRate = (*payload)["baudRate"].as<int>();
  baudRate = setSerialBaudRate(baudRate);
  Serial.begin(baudRate);

  String response = "{ \"baudRate\": ";
  response.concat(baudRate);
  response.concat(" }");
  server.send(200, "application/json", response);

  payload->clear();
  delete payload;
}



void handleNotFound() {
  server.send(404, "application/json", "{}");
}



DynamicJsonDocument* getPayload() {
  if(server.hasArg("plain") == false) {
    server.send(400, "application/json", "{ \"error\": \"Data not found.\" }");
    return NULL;
  }

  DynamicJsonDocument* doc = new DynamicJsonDocument(2048);
  if(deserializeJson(*doc, server.arg("plain"))) {
    server.send(400, "application/json", "{ \"error\": \"Invalid param.\" }");
    return NULL;
  }

  return doc;
}


/***********************************
// Main
***********************************/
void setup() {
  EEPROM.begin(128);
  
  // init serial
  int baudRate = getSerialBaudRate();
  Serial.begin(baudRate);
  Serial.setTimeout(5000);
  
  // init pins
  pinMode(PIN_0, OUTPUT);
  pinMode(PIN_1, OUTPUT);
  digitalWrite(PIN_0, LOW);
  digitalWrite(PIN_1, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);

  // init wifi
  WiFi.mode(WIFI_AP_STA);
  IPAddress apIP(192, 168, 1, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  struct Ap ap = { "", "" }; getAp(&ap);
  if(strlen(ap.ssid) <= 0) {
    WiFi.softAP(DEFAULT_AP_SSID, DEFAULT_AP_PASSWD);
  } else {
    WiFi.softAP(ap.ssid, ap.passwd);
  }
  
  // start
  startServer();
  startMDNS();

  EEPROM.end();
}



void loop() {
  server.handleClient();
}
