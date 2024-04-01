/*
  nano-serial-read - test program: read from serial interface

  20240401, Jens

  board: Arduino Nano

*/

#define txPin 7
#define rxPin 6

#include <SoftwareSerial.h>
SoftwareSerial Serial1 = SoftwareSerial(rxPin, txPin);  // add. serial i/f

unsigned long previousMillis;  // variable for comparing millis counter

char line[256];
byte linePtr = 0;


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


  // input line
  linePtr = 0;

  // output
  Serial.println();
  Serial.println();
  Serial.println("nano-serial-read - test program: read from serial interface");
  Serial.println();
}


void loop() {

  // read from additional serial interface
  if (Serial1.available()) {

    char c = Serial1.read();

    if ((c == '\n') || (c == '\r')) {  // NL or CR
      line[linePtr] = 0;
      Serial.print("-->");
      Serial.print(line);
      Serial.print("==>"); 
      for (byte i = 0; line[i] != 0; i++) {
        Serial.print(" 0x");
        Serial.print(line[i], HEX);
      }
      Serial.println();
      linePtr = 0;  // start new command line
    } else {
      line[linePtr] = c;
      linePtr++;    // long line wrap ptr
    }
  }

  // toogle LED in main loop
  if (millis() - previousMillis >= 1000) {  // every second
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    previousMillis = millis();
  }
}
