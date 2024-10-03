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

#include <Joystick.h>

#define BTN_1 2
#define BTN_2 3
#define BTN_3 4
#define BTN_4 5
#define BTN_5 6
#define BTN_6 7
#define BTN_7 8
#define BTN_8 9
#define BTN_9 10
#define BTN_10 16

#define BTN_UP A0
#define BTN_RIGHT A1
#define BTN_DOWN A2
#define BTN_LEFT A3


// Global variables
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
  10, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering;

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

  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
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

  if(up != prevUp || down != prevDown) {
    prevUp = up;
    prevDown = down;
    Joystick.setYAxis(up == LOW ? -1 : (down == LOW ? 1 : 0));
  }
  if(right != prevRight || left != prevLeft) {
    prevRight = right;
    prevLeft = left;
    Joystick.setXAxis(right == LOW ? 1 : (left == LOW ? -1 : 0));
  }
  

  if(button1 != prevBtn1) {
    prevBtn1 = button1;
    Joystick.setButton(0, !button1);
  }
  if(button2 != prevBtn2) {
    prevBtn2 = button2;
    Joystick.setButton(1, !button2);
  }
  if(button3 != prevBtn3) {
    prevBtn3 = button3;
    Joystick.setButton(2, !button3);
  }
  if(button4 != prevBtn4) {
    prevBtn4 = button4;
    Joystick.setButton(3, !button4);
  }
  if(button5 != prevBtn5) {
    prevBtn5 = button5;
    Joystick.setButton(4, !button5);
  }
  if(button6 != prevBtn6) {
    prevBtn6 = button6;
    Joystick.setButton(5, !button6);
  }
  if(button7 != prevBtn7) {
    prevBtn7 = button7;
    Joystick.setButton(6, !button7);
  }
  if(button8 != prevBtn8) {
    prevBtn8 = button8;
    Joystick.setButton(7, !button8);
  }
  if(button9 != prevBtn9) {
    prevBtn9 = button9;
    Joystick.setButton(8, !button9);
  }
  if(button10 != prevBtn10) {
    prevBtn10 = button10;
    Joystick.setButton(9, !button10);
  }

  delay(10);
}
