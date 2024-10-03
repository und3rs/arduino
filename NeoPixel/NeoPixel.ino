#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 30
#define NUM_PIXELS 12

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  // put your setup code here, to run once:
  pixels.begin();
  pixels.setBrightness(50);
  pixels.clear();
  pixels.show();
}


void loop() {
  uint32_t colors[12];
  colors[0] = pixels.Color(255, 64, 0);
  colors[1] = pixels.Color(0, 255, 0);
  colors[2] = pixels.Color(0, 0, 255);
  colors[3] = pixels.Color(255, 255, 0);
  colors[4] = pixels.Color(0, 255, 255);
  colors[5] = pixels.Color(255, 0, 255);
  colors[6] = pixels.Color(255, 127, 0);
  colors[7] = pixels.Color(0, 255, 127);
  colors[8] = pixels.Color(127, 0, 255);
  colors[9] = pixels.Color(255, 0, 255);
  colors[10] = pixels.Color(127, 64, 255);
  colors[11] = pixels.Color(255, 255, 64);

  for(uint16_t i = 0; i < pixels.numPixels(); i++) {
    colorWheel(&pixels, colors[i], 40, i); // Red
  }
}


void colorWheel(Adafruit_NeoPixel* leds, uint32_t color, uint16_t wait, uint16_t idx) {
  for(uint16_t i=0; i < (*leds).numPixels(); i++){ 
    if(i == idx) { 
      (*leds).setPixelColor(i, (*leds).Color(255, 0, 0));
    } else {
      (*leds).setPixelColor(i, color);
    }
    (*leds).show();
    delay(wait);
  }
}