/*
  nano-serial-write - test program: write to serial interface

  20240401, Jens

  board: Arduino Nano

*/

#define txPin 7
#define rxPin 6

#include <SoftwareSerial.h>
SoftwareSerial Serial1 = SoftwareSerial(rxPin, txPin);  // add. serial i/f

unsigned long previousMillis;  // variable for comparing millis counter
unsigned long period = 1000;   // write period 

int counter = 1;


void setup() {

  // status led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  previousMillis = millis();

  // usb serial interface
  Serial.begin(9600);

  // additional serial interface
  pinMode(rxPin, INPUT);  // pins serial interface
  pinMode(txPin, OUTPUT);
  Serial1.begin(9600);

  // output
  Serial.println();
  Serial.println("nano-serial-write - test program: write to serial interface");
  Serial.println();

  Serial1.println("---serial-writes---");
}


void loop() {

  // write to additional serial 
  if (millis() - previousMillis >= period) {
     previousMillis = millis();

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // toggle led
    
    Serial1.print("message ");
    Serial1.println(counter);

    Serial.print("count ");
    Serial.println(counter);

    counter++;
  }

  // read from additional serial interface
  if (Serial1.available()) {
    char c = Serial1.read();

    Serial.print(c);
    Serial.print("(0x");
    Serial.print(c, HEX);
    Serial.print(") ");
  }
}



