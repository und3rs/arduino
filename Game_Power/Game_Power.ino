// Set Environments - START
#define PIN_SPEAKER 4
#define PIN_NEO_PIXEL 8 // On Trinket or Gemma, suggest changing this to 1
#define PIN_BTN_1 2
#define PIN_BTN_2 3

#define NUMPIXELS 12 // Popular NeoPixel ring size
//#define DEBUGGING true
// Set Environments - END

#define STATE_STARTING 1
#define STATE_RUNNING 2
#define STATE_END 3

#include "pitches.h"  // Speaker
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// How many NeoPixels are attached to the Arduino?
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 200 // Time (in milliseconds) to pause between pixels




// notes in the melody:
// int startingMelody[] = {
//   NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
// };

int startingMelody[] = {
  NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};


volatile long score = NUMPIXELS * 2;
volatile long position = NUMPIXELS;
uint32_t colorOfUser1 = pixels.Color(0, 255, 0);
uint32_t colorOfUser2 = pixels.Color(255, 0, 0);
uint32_t colorOfCenter = pixels.Color(255, 255, 0);
int state = STATE_STARTING;
int winner = 0;




void initNeoPixel() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(10);
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
}


void initSerial() {
  Serial.begin(9600);
}


void initButtons() {
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_BTN_1), onButton1Pressed, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_2), onButton2Pressed, RISING);
}


void runStartingEffect() {
  pixels.clear();
  pixels.show();
  for(int i = 0; i < NUMPIXELS; i++) {
    tone(PIN_SPEAKER, startingMelody[i], DELAYVAL/2);

    pixels.setPixelColor(i, pixels.Color(150, 0, 150));
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(DELAYVAL/2); // Pause before next pass through loop
  }
  pixels.clear();
  pixels.show();
  delay(1000);

  tone(PIN_SPEAKER, startingMelody[0], 1000);
  pixels.fill(pixels.Color(150, 0, 0));
  pixels.show();
  delay(1000);

  pixels.clear();
  pixels.show();
  delay(100);

  tone(PIN_SPEAKER, startingMelody[0], 1000);
  pixels.fill(pixels.Color(0, 150, 0));
  pixels.show();
  delay(1000);

  pixels.clear();
  pixels.show();
  delay(100);

  tone(PIN_SPEAKER, startingMelody[0], 1000);
  pixels.fill(pixels.Color(0, 0, 150));
  pixels.show();
  delay(1000);

  pixels.clear();
  pixels.show();
  delay(100);

  tone(PIN_SPEAKER, startingMelody[6], 1000);
  pixels.fill(pixels.Color(150, 150, 150));
  pixels.show();
  delay(1000);

  pixels.clear();
  pixels.show();

}


void displayWinner(int user) {
  uint32_t color = (user == 1 ? colorOfUser1 : (user == 2 ? colorOfUser2 : pixels.Color(255, 255, 255)));
  pixels.fill(color);
  pixels.show();
  tone(PIN_SPEAKER, startingMelody[6], 500);
  delay(500);
  noTone(PIN_SPEAKER);

  pixels.clear();
  pixels.show();
  delay(300);

  pixels.fill(color);
  pixels.show();
  tone(PIN_SPEAKER, startingMelody[6], 500);
  delay(500);
  noTone(PIN_SPEAKER);

  pixels.clear();
  pixels.show();
  delay(300);

  pixels.fill(color);
  pixels.show();
  tone(PIN_SPEAKER, startingMelody[6], 500);
  delay(1000);
  noTone(PIN_SPEAKER);
}


void initGame() {
  runStartingEffect();

  score = (NUMPIXELS*2) / 2;
  position = score/2;
  winner = 0;
}




void onButton1Pressed() {
  if(state != STATE_RUNNING) { return; }
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 30) {
    score--;
    position = score / 2;
    #ifdef DEBUGGING
    Serial.print("+onButton1Pressed: ");
    Serial.println(score);
    #endif
  }
  last_interrupt_time = interrupt_time;
}


void onButton2Pressed() {
  if(state != STATE_RUNNING) { return; }
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 30) {
    score++;
    position = score / 2;
    #ifdef DEBUGGING
    Serial.print("+onButton2Pressed: ");
    Serial.println(score);
    #endif
  }
  last_interrupt_time = interrupt_time;
}


void setup() {
  initSerial();
  initButtons();
  initNeoPixel();
}




void loop() {
  switch(state) {
    case STATE_STARTING: {
      initGame();
      state = STATE_RUNNING;
      break;
    }
    case STATE_RUNNING: {
      int user1Finished = position <= 0;
      int user2Finished = position >= NUMPIXELS;

      pixels.clear();
      for(int i = 0; i < position; i++) {
        pixels.setPixelColor(i, colorOfUser2);
      }
      pixels.setPixelColor(position, colorOfCenter);
      for(int i = position+1; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, colorOfUser1);
      }
      pixels.show();

      if(user1Finished) {
        #ifdef DEBUGGING
        Serial.println("++ FINISHED: DRAW");
        #endif
        state = STATE_END;
        winner = 1;
      } else if(user2Finished) {
        #ifdef DEBUGGING
        Serial.println("++ FINISHED: User1 Win.");
        #endif
        state = STATE_END;
        winner = 2;
      }
      break;
    }
    case STATE_END: {
      displayWinner(winner);
      state = STATE_STARTING;
      break;
    }
  }
  
  
  delay(10);
}
