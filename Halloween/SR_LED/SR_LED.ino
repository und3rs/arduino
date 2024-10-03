#include <SoftwareSerial.h>

int trigPin = 0;
int echoPin = 1;
int ledPin = 2; 

//SoftwareSerial monitor(3, 4);

void setup() {
  // put your setup code here, to run once:
  //monitor.begin(4800);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  long duration, distance;

  digitalWrite(trigPin, HIGH);
  delayMicrosecond`s(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = ((float)(340 * duration) / 1000) / 2;


  // monitor.print("Duration:");
  // monitor.print(duration);
  // monitor.print("\nDistance:");
  // monitor.print(distance);
  // monitor.println("mm\n");

  if(distance <= 800) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  delay(200);
}
