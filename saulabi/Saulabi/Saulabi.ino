/*
  SAULABI

  FOR ARDUINO NANO
  * PINMAP *
  D1/TX  (1) | | (30) VIN
  D0/RX  (2) | | (29) GND
  RESET  (3) | | (28) RESET
    GND  (4) | | (27) +5V
     D2  (5) | | (26) A7
     D3  (6) | | (25) A6
     D4  (7) | | (24) A5
     D5  (8) | | (23) A4
     D6  (9) | | (22) A3
     D7 (10) | | (21) A2
     D8 (11) | | (20) A1
     D9 (12) | | (19) A0
     D10(13) | | (18) AREF
     D11(14) | | (17) 3V3
     D12(15) | | (16) D13
*/

#include <Keyboard.h>

#define BTN_1 2
#define BTN_2 3
#define BTN_3 4
#define BTN_4 5
#define BTN_5 6
#define BTN_6 7
#define BTN_7 8
#define BTN_8 9
#define BTN_9 10
#define BTN_10 11

#define BTN_UP A0
#define BTN_RIGHT A1
#define BTN_DOWN A2
#define BTN_LEFT A3

#define BTN_1_KEY 'Q'
#define BTN_2_KEY 'W'
#define BTN_3_KEY 'A'
#define BTN_4_KEY 'S'
#define BTN_5_KEY 'D'
#define BTN_6_KEY 'F'
#define BTN_7_KEY 'Z'
#define BTN_8_KEY 'X'
#define BTN_9_KEY 'C'
#define BTN_10_KEY 'V'



// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(BTN_5, INPUT_PULLUP);
  pinMode(BTN_6, INPUT_PULLUP);
  pinMode(BTN_7, INPUT_PULLUP);
  pinMode(BTN_8, INPUT_PULLUP);
  pinMode(BTN_9, INPUT_PULLUP);
  pinMode(BTN_10, INPUT_PULLUP);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);

  Serial.begin(9600);
}


int prevUp = HIGH;
int prevRight = HIGH;
int prevDown = HIGH;
int prevLeft = HIGH;

int prevBtn1 = HIGH;
int prevBtn2 = HIGH;
int prevBtn3 = HIGH;
int prevBtn4 = HIGH;
int prevBtn5 = HIGH;
int prevBtn6 = HIGH;
int prevBtn7 = HIGH;
int prevBtn8 = HIGH;
int prevBtn9 = HIGH;
int prevBtn10 = HIGH;

// the loop function runs over and over again forever
void loop() {
  int up = digitalRead(BTN_UP);
  int right = digitalRead(BTN_RIGHT);
  int down = digitalRead(BTN_DOWN);
  int left = digitalRead(BTN_LEFT);

  int button1 = digitalRead(BTN_1);
  int button2 = digitalRead(BTN_2);
  int button3 = digitalRead(BTN_3);
  int button4 = digitalRead(BTN_4);
  int button5 = digitalRead(BTN_5);
  int button6 = digitalRead(BTN_6);
  int button7 = digitalRead(BTN_7);
  int button8 = digitalRead(BTN_8);
  int button9 = digitalRead(BTN_9);
  int button10 = digitalRead(BTN_10);

  // if(up == LOW) Keyboard.press(KEY_UP_ARROW); else Keyboard.release(KEY_UP_ARROW);
  // if(right == LOW) Keyboard.press(KEY_RIGHT_ARROW); else Keyboard.release(KEY_RIGHT_ARROW);
  // if(down == LOW) Keyboard.press(KEY_DOWN_ARROW); else Keyboard.release(KEY_DOWN_ARROW);
  // if(left == LOW) Keyboard.press(KEY_LEFT_ARROW); else Keyboard.release(KEY_LEFT_ARROW);

  // if(button1 == LOW) Keyboard.press(BTN_1_KEY); else Keyboard.release(BTN_1_KEY);
  // if(button2 == LOW) Keyboard.press(BTN_2_KEY); else Keyboard.release(BTN_2_KEY);
  // if(button3 == LOW) Keyboard.press(BTN_3_KEY); else Keyboard.release(BTN_3_KEY);
  // if(button4 == LOW) Keyboard.press(BTN_4_KEY); else Keyboard.release(BTN_4_KEY);
  // if(button5 == LOW) Keyboard.press(BTN_5_KEY); else Keyboard.release(BTN_5_KEY);
  // if(button6 == LOW) Keyboard.press(BTN_6_KEY); else Keyboard.release(BTN_6_KEY);
  // if(button7 == LOW) Keyboard.press(BTN_7_KEY); else Keyboard.release(BTN_7_KEY);
  // if(button8 == LOW) Keyboard.press(BTN_8_KEY); else Keyboard.release(BTN_8_KEY);
  // if(button9 == LOW) Keyboard.press(BTN_9_KEY); else Keyboard.release(BTN_9_KEY);
  // if(button10 == LOW) Keyboard.press(BTN_10_KEY); else Keyboard.release(BTN_10_KEY);


  if(up != prevUp) {
    prevUp = up;
    if(up == LOW) Serial.println("+KEY_UP_ARROW"); else Serial.println("-KEY_UP_ARROW");
  }
  if(right != prevRight) {
    prevRight = right;
    if(right == LOW) Serial.println("+KEY_RIGHT_ARROW"); else Serial.println("-KEY_RIGHT_ARROW");
  }
  if(down != prevDown) {
    prevDown = down;
    if(down == LOW) Serial.println("+KEY_DOWN_ARROW"); else Serial.println("-KEY_DOWN_ARROW");
  }
  if(left != prevLeft) {
    prevLeft = left;
    if(left == LOW) Serial.println("+KEY_LEFT_ARROW"); else Serial.println("-KEY_LEFT_ARROW");
  }
  if(button1 != prevBtn1) {
    prevBtn1 = button1;
    if(button1 == LOW) Serial.println("+BTN_1_KEY"); else Serial.println("-BTN_1_KEY");
  }
  if(button2 != prevBtn2) {
    prevBtn2 = button2;
    if(button2 == LOW) Serial.println("+BTN_2_KEY"); else Serial.println("-BTN_2_KEY");
  }
  if(button3 != prevBtn3) {
    prevBtn3 = button3;
    if(button3 == LOW) Serial.println("+BTN_3_KEY"); else Serial.println("-BTN_3_KEY");
  }
  if(button4 != prevBtn4) {
    prevBtn4 = button4;
    if(button4 == LOW) Serial.println("+BTN_4_KEY"); else Serial.println("-BTN_4_KEY");
  }
  if(button5 != prevBtn5) {
    prevBtn5 = button5;
    if(button5 == LOW) Serial.println("+BTN_5_KEY"); else Serial.println("-BTN_5_KEY");
  }
  if(button6 != prevBtn6) {
    prevBtn6 = button6;
    if(button6 == LOW) Serial.println("+BTN_6_KEY"); else Serial.println("-BTN_6_KEY");
  }
  if(button7 != prevBtn7) {
    prevBtn7 = button7;
    if(button7 == LOW) Serial.println("+BTN_7_KEY"); else Serial.println("-BTN_7_KEY");
  }
  if(button8 != prevBtn8) {
    prevBtn8 = button8;
    if(button8 == LOW) Serial.println("+BTN_8_KEY"); else Serial.println("-BTN_8_KEY");
  }
  if(button9 != prevBtn9) {
    prevBtn9 = button9;
    if(button9 == LOW) Serial.println("+BTN_9_KEY"); else Serial.println("-BTN_9_KEY");
  }
  if(button10 != prevBtn10) {
    prevBtn10 = button10;
    if(button10 == LOW) Serial.println("+BTN_10_KEY"); else Serial.println("-BTN_10_KEY");
  }

  delay(10);
}
