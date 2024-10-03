#include <avr/io.h>
#include <avr/interrupt.h>
#include <SimpleTimer.h>
#include <avr/sleep.h>

#define PIN_PLUS PB3
#define PIN_MINUS PB4
#define PIN_START PB2
#define PIN_LED PB0

#define TEIMER_SEC 1800

SimpleTimer timer;
int timerId = 0;
volatile int seconds = 0;
volatile bool isRunning = false;

#define MAX_BRIGHTNESS 16
const int BRIGHTNESS[17] = {15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 255};
volatile int brightnessIdx = MAX_BRIGHTNESS;
volatile unsigned long prevEventTime = 0L;


void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PLUS, INPUT_PULLUP);
  pinMode(PIN_MINUS, INPUT_PULLUP);
  pinMode(PIN_START, INPUT_PULLUP);

  analogWrite(PIN_LED, 0);
  
  cli();
  GIMSK = 0b00100000;
  PCMSK = 0b00011100;
  sei();

  timerId = timer.setInterval(1000, tick);
}
 

void loop() {
  timer.run();
}




void tick() {
  if(isRunning) {
    seconds++;
    analogWrite(PIN_LED, calcBrightness());
  
    if(seconds > TEIMER_SEC) {
      stopTimer();
    }
  }
}


void toggleTimer() {
  if(!isRunning) {
    startTimer();
  } else {
    stopTimer();
  }
}


void startTimer() {
  if(!isRunning) {
    seconds = 0;
    analogWrite(PIN_LED, calcBrightness());
    isRunning = true;
  }
}


void stopTimer() {
  if(isRunning) {
    isRunning = false;
    analogWrite(PIN_LED, 0);
    
    sleepMode();
  }
}



//=========================================================
// Setting Functions
//=========================================================
void ledPlus() {
  brightnessIdx = min(brightnessIdx+1, MAX_BRIGHTNESS);
  if(isRunning) {
    analogWrite(PIN_LED, calcBrightness());
  }
}


void ledMinus() {
  brightnessIdx = max(brightnessIdx-1, 0);
  if(isRunning) {
    analogWrite(PIN_LED, calcBrightness());
  }
}


int calcBrightness() {
  int brightness = BRIGHTNESS[brightnessIdx];
  return int(brightness * (1.0f - (seconds / float(TEIMER_SEC))));
}



//=========================================================
// Power SAVE
//=========================================================
void sleepMode()
{
    timer.disable(timerId);
    cli();                               // Disable Interrupts before next commands    
    sleep_enable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode power down
    sei();               // Enable interrupts
    sleep_cpu();
    sleep_mode();
    
    sleep_disable();
    timer.enable(timerId);
}



ISR(PCINT0_vect)
{
  unsigned long currentTime = millis();
  if(currentTime - prevEventTime > 200) {
    
    if(digitalRead(PIN_START) == LOW) {
      toggleTimer();
    } else if(digitalRead(PIN_PLUS) == LOW) {
      ledPlus();
    } else if(digitalRead(PIN_MINUS) == LOW) {
      ledMinus();
    }

    prevEventTime = millis();
  }
}
