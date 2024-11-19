#include <avr/sleep.h>
#include "SimpleTimer.h"


// #define PIN_LED 3
// #define PIN_UP A0
// #define PIN_DOWN A1
// #define PIN_POWER 2

#define PIN_UP PB3
#define PIN_DOWN PB4
#define PIN_POWER PB2
#define PIN_LED PB0


#define TIME_OUT 1200

//#define DEBUG 1

volatile float brightnessScale = 1.0f;
volatile float brightness = 255;

SimpleTimer timer;
volatile int timerId = 0;
int passedTimeInSec = 0;


void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);

  pinMode(PIN_POWER, INPUT_PULLUP);

  // for Arduino Nano
  //attachInterrupt(digitalPinToInterrupt(PIN_POWER), toggleOnOff, FALLING);

  // for ATTiny 45/85
  cli();
  GIMSK = 0b00100000;
  PCMSK = 0b00000100;
  sei();

  stop();
}



void loop() {
  static unsigned long last_interrupt_time = 0;

  // put your main code here, to run repeatedly:
  updateBrightness();
  analogWrite(PIN_LED, brightness);

  timer.run();

  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 100) {
    if(digitalRead(PIN_UP) == LOW) {
      brightnessScale = min(brightnessScale + 0.1f, 1.0f);
      updateBrightness();
      #ifdef DEBUG
      Serial.println(brightness);
      #endif
    } else if(digitalRead(PIN_DOWN) == LOW) {
      brightnessScale = max(brightnessScale - 0.1f, 0.1f);
      updateBrightness();
      #ifdef DEBUG
      Serial.println(brightness);
      #endif
    }
    last_interrupt_time = interrupt_time;
  }
}



void start() {
  #ifdef DEBUG
  Serial.println("\n\n>> START <<");
  #endif
  
  passedTimeInSec = 0;
  timerId = timer.setInterval(1000, tick);
  timer.enable(timerId);
  updateBrightness();
}



void stop() {
  timer.disable(timerId);
  timer.deleteTimer(timerId);
  passedTimeInSec = TIME_OUT;
  updateBrightness();
  analogWrite(PIN_LED, brightness);

  #ifdef DEBUG
  Serial.println(">> SLEEP");
  #endif
  sleep_enable();  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();

  // woke up
  start();
  #ifdef DEBUG
  Serial.println(">> WOKEUP");
  #endif
  sleep_disable(); 
}



void updateBrightness() {
  brightness = ((255.0 * ((TIME_OUT - passedTimeInSec) / float(TIME_OUT))) * brightnessScale);
}



void tick() {
  #ifdef DEBUG
  Serial.print("[+Tick]");
  #endif

  passedTimeInSec++;
  #ifdef DEBUG
  Serial.print("[Time]: ");
  Serial.println(passedTimeInSec);
  #endif

  if(passedTimeInSec >= TIME_OUT) {
    #ifdef DEBUG
    Serial.println(">> STOP");
    #endif
    stop();
  }
}



void toggleOnOff()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 500) {
    if(passedTimeInSec > 0) {
      #ifdef DEBUG
      Serial.println("[BUTTON] STOP");
      #endif
      passedTimeInSec = TIME_OUT;
    }
  }
}


ISR(PCINT0_vect)
{
  toggleOnOff();
}