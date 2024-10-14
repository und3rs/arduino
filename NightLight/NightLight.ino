#include <avr/io.h>
#include <avr/interrupt.h>
#include "SimpleTimer.h"
#include <avr/sleep.h>

#define PIN_PLUS PB3
#define PIN_MINUS PB4
#define PIN_START PB2
#define PIN_LED PB0

#define TEIMER_SEC 10

SimpleTimer timer;
int timerId = 0;
volatile int seconds = 0;
volatile bool isRunning = false;

#define MAX_BRIGHTNESS 6
const int BRIGHTNESS[7] = {5, 15, 50, 80, 110, 180, 255};
volatile int brightnessIdx = MAX_BRIGHTNESS;


void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PLUS, INPUT_PULLUP);
  pinMode(PIN_MINUS, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);
  
  cli();
  GIMSK = 0b00100000;
  PCMSK = 0b00011100;
  sei();

  analogWrite(PIN_LED, 0);
  timer.disable(timerId);
  timerId = timer.setInterval(1000, tick);
  isRunning = false;
}
 


void loop() {
  if(isRunning) {
    timer.run();
  }
}



void tick() {
  if(isRunning) {
    seconds++;

    if(seconds >= TEIMER_SEC) {
      sleepMode();
    } else {
      analogWrite(PIN_LED, calcBrightness());
    }
  }
}




//=========================================================
// Setting Functions
//=========================================================
void ledPlus() {
  if(isRunning) {
    brightnessIdx = min(brightnessIdx+1, MAX_BRIGHTNESS);
    analogWrite(PIN_LED, calcBrightness());
  }
}


void ledMinus() {
  if(isRunning) {
    brightnessIdx = max(brightnessIdx - 1, 0);
    analogWrite(PIN_LED, calcBrightness());
  }
}


int calcBrightness() {
  int brightness = BRIGHTNESS[brightnessIdx];
  return max(int(brightness * (1.0f - (seconds / float(TEIMER_SEC)))), BRIGHTNESS[0]);
}



//=========================================================
// Power SAVE
//=========================================================
void sleepMode()
{
  isRunning = false;
  analogWrite(PIN_LED, 0);
  timer.disable(timerId);

  
  // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // sleep_enable();
  // sleep_mode();

  // sleep_disable();
}



void wakeUp() {
  seconds = 0;
  isRunning = true;
  timer.enable(timerId);
  analogWrite(PIN_LED, calcBrightness());
}



ISR(PCINT0_vect)
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 500) {
    if(digitalRead(PIN_START) == LOW) {
      if(!isRunning) {
        wakeUp();
      } else {
        sleepMode();
      }
    } else if(digitalRead(PIN_PLUS) == LOW) {
      ledPlus();
    } else if(digitalRead(PIN_MINUS) == LOW) {
      ledMinus();
    }
    last_interrupt_time = interrupt_time;
  }
}
