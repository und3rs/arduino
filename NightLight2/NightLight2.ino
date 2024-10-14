#include <avr/sleep.h>
#include "SimpleTimer.h"

#define PIN_LED 3
#define PIN_UP A0
#define PIN_DOWN A1

#define TIME_OUT 5

#define DEBUG 1

volatile float brightnessScale = 1.0f;
volatile float brightness = 255;

SimpleTimer timer;
volatile int timerId = 0;
int passedTimeInSec = 0;
volatile bool isRunning = false;


void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), toggleOnOff, FALLING);

  stop();
}



void loop() {
  static unsigned long last_interrupt_time = 0;

  // put your main code here, to run repeatedly:
  updateBrightness();
  analogWrite(PIN_LED, brightness);
  if(isRunning == false) { return; }

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
  if(isRunning == false) {
    #ifdef DEBUG
    Serial.println("Start()");
    #endif
    isRunning = true;
    passedTimeInSec = 0;
    timerId = timer.setInterval(1000, tick);
    timer.enable(timerId);
    updateBrightness();
  }
}



void stop() {
  if(isRunning == true) {
    #ifdef DEBUG
    Serial.println("Stop()");
    #endif
    isRunning = false;
    timer.disable(timerId);
    timer.deleteTimer(timerId);
    passedTimeInSec = TIME_OUT;
    updateBrightness();
    analogWrite(PIN_LED, brightness);
  }
}



int updateBrightness() {
  brightness = isRunning ? ((255.0 * ((TIME_OUT - passedTimeInSec) / float(TIME_OUT))) * brightnessScale) : 0;
}



void tick() {
  Serial.print("[+Tick]");
  if(isRunning) {
    passedTimeInSec++;
    #ifdef DEBUG
    Serial.print("[Time]: ");
    Serial.println(passedTimeInSec);
    #endif

    if(passedTimeInSec >= TIME_OUT && isRunning) {
      stop();
    }
  }
}



void toggleOnOff()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 500) {
    !isRunning ? start() : stop();
    last_interrupt_time = interrupt_time;
  }
}