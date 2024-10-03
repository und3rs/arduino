#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Wire.h>
#include <RtcDS3231.h>

/***********************************
// ENVIRONMENTS
***********************************/
#define PIN_RX 3
#define PIN_TX 4
#define PIN_PWM 0
#define OFFSET_OF_BRIGHTNESS 0
#define OFFSET_OF_ALARM 8
#define OFFSET_OF_TURN_ON 12
#define OFFSET_OF_TURN_OFF 44


#define countof(a) (sizeof(a) / sizeof(a[0]))


SoftwareSerial serial(PIN_RX, PIN_TX);
RtcDS3231<TwoWire> Rtc(Wire);


/***********************************
// Settings
***********************************/
String getLatestBrightness() {
  int brightness = 0;
  //EEPROM.begin(8);
  EEPROM.get(OFFSET_OF_BRIGHTNESS, brightness);
  //EEPROM.end();
  return String(max(min(brightness, 255), 0));
}


String setLatestBrightness(String brightness) {
  int saftyBrightness = max(min(brightness.toInt(), 255), 0);
  //EEPROM.begin(8);
  EEPROM.put(OFFSET_OF_BRIGHTNESS, saftyBrightness);
  //EEPROM.commit();
  //EEPROM.end();
  return String(saftyBrightness);
}


/***********************************
// Handlers
***********************************/
#define CMD_UNKNOWN -1
#define CMD_LED 0
#define CMD_CLK 1
#define CMD_BRT 2
#define CMD_ALM 3

int commandToInt(String command) {
  if(command == "LED") {
    return CMD_LED;
  } else if(command == "CLK") {
    return CMD_CLK;
  } else if(command == "BRT") {
    return CMD_BRT;
  } else if(command == "ALM") {
    return CMD_ALM;
  }
  return CMD_UNKNOWN;
}



String handleLed(String value) {
  int saftyValue = value.toInt();
  saftyValue = max(min(saftyValue, 255), 0);

  analogWrite(PIN_PWM, saftyValue);
  return setLatestBrightness(String(saftyValue));
}



String handleGetClock() {
  RtcDateTime now = Rtc.GetDateTime();
  char datestring[20];
  snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    now.Month(),
    now.Day(),
    now.Year(),
    now.Hour(),
    now.Minute(),
    now.Second()
  );
  return String(datestring);
}



String handleSetClock(String value) {
  return String("Not Implemented");
}



String handleSetAlarm(String value) {
  if(value == "-") {
    EEPROM.put(OFFSET_OF_ALARM, "no");
    return String("Deleted.");
  }
  
  RtcDateTime turnOn = RtcDateTime("Jan 1 2022", value.c_str());
  EEPROM.put(OFFSET_OF_TURN_ON, turnOn);

  RtcDateTime turnOff = RtcDateTime("Jan 1 2022", value.c_str());
  EEPROM.put(OFFSET_OF_TURN_OFF, turnOff);

  EEPROM.get(OFFSET_OF_ALARM, "set");
  return String("Set.");
}



String handleGetAlarm() {
  char datestring[20];
  RtcDateTime alarm;

  char hasAlarm[4];
  EEPROM.get(OFFSET_OF_ALARM, hasAlarm);
  if(!strcmp(hasAlarm, "set")) {
    return String("-");
  }
  
  String result = String("On: ");
  
  EEPROM.get(OFFSET_OF_TURN_ON, alarm);
  snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    alarm.Month(),
    alarm.Day(),
    alarm.Year(),
    alarm.Hour(),
    alarm.Minute(),
    alarm.Second()
  );
  result.concat(datestring);
  result.concat(", Off: ");
  
  EEPROM.get(OFFSET_OF_TURN_OFF, alarm);
  snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    alarm.Month(),
    alarm.Day(),
    alarm.Year(),
    alarm.Hour(),
    alarm.Minute(),
    alarm.Second()
  );
  result.concat(datestring);
  
  return result;
}



void checkSchedule() {
  // turn on or off
}


/***********************************
// Main
***********************************/
void setup() {
  serial.begin(9600);
  serial.setTimeout(10);
  
  // init pins
  pinMode(PIN_PWM, OUTPUT);
  analogWrite(PIN_PWM, 0);  // max 255

  // init rtc
  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }
}



void loop() {
  // check schedule
  checkSchedule();

  // communicate with ESP8266
  while(serial.available()) {
    String data = serial.readString();
    if(data.length() < 5) { continue; }

    char sign = data.charAt(3);
    if(sign != ':') { continue; }

    String command = data.substring(0, 3);
    String value = data.substring(4, data.length()-1);
    switch(commandToInt(command)) {
      case CMD_LED: serial.print(handleLed(value)+"\n"); break;
      case CMD_CLK: {
        if(value.length() > 0) {
          serial.println(handleSetClock(value)+"\n");
        } else {
          serial.print(handleGetClock()+"\n");
        }
        break;
      }
      case CMD_BRT: {
        if(value.length() > 0) {
          serial.println(setLatestBrightness(value)+"\n");
        } else {
          serial.print(getLatestBrightness()+"\n");
        }
        break;
      }
      case CMD_ALM: {
        if(value.length() > 0) {
          serial.println(handleSetAlarm(value)+"\n");
        } else {
          serial.print(handleGetAlarm()+"\n");
        }
        break;
      }
      default: serial.print("Unknown Message\n"); break;
    }
  }
}
