/*
 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 Steps:
 1. Connect PIN_NEXT_BAUDRATE with GND by switch.
 2. Connect local devices's GND with remote device's GND.
 3. Connect PIN_RX with remote device's TX.
 4. Connect PIN_TX with remote device's RX.
 5. Open Terminal App and connect COM port of local device with BAUDRATE_OF_LOCAL.
 */
#include <SoftwareSerial.h>

#define PIN_NEXT_BAUDRATE 5
#define PIN_RX 10
#define PIN_TX 11
#define BAUDRATE_OF_LOCAL 57600

SoftwareSerial remoteSerial(PIN_RX, PIN_TX); // RX, TX

unsigned long boudarates[] = {2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200, 230400, 250000};
int boudrateIndex = 0;
int countOfBoudrates = sizeof(boudarates) / sizeof(boudarates[0]);


void setup() {
  pinMode(PIN_NEXT_BAUDRATE, INPUT_PULLUP);

  Serial.begin(BAUDRATE_OF_LOCAL);
  while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("[SYSTEM] Started.");

  changeBaudrateToNext();
}



void changeBaudrateToNext() {
  boudrateIndex = (boudrateIndex + 1) % countOfBoudrates;
  changeBaudrate(boudarates[boudrateIndex]);
}



void changeBaudrate(unsigned long rate) {
  remoteSerial.begin(rate);
  Serial.print("[SYSTEM] Boudrate Changed: ");
  Serial.println(rate);
}


int prevBtnNextBaudrate = HIGH;
void loop() { // run over and over  
  int btnNextBaudrate = digitalRead(PIN_NEXT_BAUDRATE);
  if(btnNextBaudrate != prevBtnNextBaudrate) {
    prevBtnNextBaudrate = btnNextBaudrate;
    if(btnNextBaudrate == LOW) {
      changeBaudrateToNext();
    }
  }


  if (remoteSerial.available()) {
    Serial.write(remoteSerial.read());
  }

  if (Serial.available()) {
     String response = Serial.readStringUntil('\n');
     long newBaudrate = getBaudrate(response);
     if(newBaudrate > 0) {
      changeBaudrate(newBaudrate);
     }
  }

  delay(10);
}




long getBaudrate(String text) {
  int index = text.indexOf(':');
  if(index < 0) { return 0; }

  if(!String("baudrate").equals(text.substring(0, index))) { return 0; }
  String command = text.substring(index + 1);
  return command.toInt();
}
