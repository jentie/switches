/*
  nano-rgb-ser - RGB LED with serial interface

  20240323, Jens

  control a RGB led via ASCII commands
  additional analog output and input

  notes:
  * led pins: D2, D3, D4
  * analog pins Ain - A0/D14 & Aout D9
  * serial interfaces: USB & Tx/D7 / Rx/D6

  notes on Nano clone programming:
  * install / reinstall CH340 driver?
  * select old bootloader?

*/


#define txPin 7
#define rxPin 6

// Ardunio Nano
#include <SoftwareSerial.h>
SoftwareSerial Serial1 = SoftwareSerial(rxPin, txPin);  // add. serial i/f
#define A_IN A0
#define A_OUT 9

// ATtiny167
// #define A_IN A12
// #define A_OUT 4



#define ON HIGH
#define OFF LOW

#define LED_R 2
#define LED_G 3
#define LED_B 4

unsigned long previousMillis;  // variable for comparing millis counter

char cmdUSB[16];  // command line from USB
byte cmdUSBPtr = 0;

char cmdSer[16];  // command line from serial interface
byte cmdSerPtr = 0;

bool echo = false;
bool debug = false;  // note: debug messages only via USB


#define HELP_DSC "\nnano-rgb-ser - RGB LED with serial interface\nversion 20240323"
#define HELP_CMD "\
commands: test, echo, debug, info, reset, ok, err\n\
colors: red, green, blue, yellow, magenta, cyan, white, black\n\
analog: out <num> (0...1023), in, loop (stop with 'q')"


void (*resetFunc)(void) = 0;  //declare reset function @ address 0


void test(int dtime) {

  // show each LED

  digitalWrite(LED_R, ON);
  delay(dtime);
  digitalWrite(LED_R, OFF);

  digitalWrite(LED_G, ON);
  delay(dtime);
  digitalWrite(LED_G, OFF);

  digitalWrite(LED_B, ON);
  delay(dtime);
  digitalWrite(LED_B, OFF);
}


void analogIn() {

  int value = analogRead(A_IN);

  Serial.print(value);
  Serial.print(" - 0x");
  Serial.println(value, HEX);

  Serial1.print(value);
  Serial1.print(" - 0x");
  Serial1.println(value, HEX);
}


void analogLoop() {

  while (true) {  // run forever

    int value = analogRead(A_IN);

    Serial.print(value);
    Serial.print(" - 0x");
    Serial.println(value, HEX);

    Serial1.print(value);
    Serial1.print(" - 0x");
    Serial1.println(value, HEX);

    delay(1000);  // every second

    if (Serial.available()) {  // stop with 'q' or '^C'
      char c = Serial.read();
      if ((c == 'q' || c == 0x03))
        break;
    }

    if (Serial1.available()) {  // stop with 'q' or '^C'
      char c = Serial1.read();
      if ((c == 'q' || c == 0x03))
        break;
    }
  }  // while
}


void analogOut(char command[]) {
  int value = 0;

  String number = command + 4;
  number.trim();

  if (debug) {
    Serial.print("value: '");
    Serial.print(number);
  }

  value = number.toInt();

  if (debug) {
    Serial.print("' - ");
    Serial.println(value);
  }

  if (value < 0)
    value = 0;
  else if (value > 1023)
    value = 1023;
  analogWrite(A_OUT, value);
}


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  previousMillis = millis();

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);  // Nano pins
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_R, OFF);
  digitalWrite(LED_G, OFF);  // default: OFF
  digitalWrite(LED_B, OFF);

  analogWrite(A_OUT, 0);  // default: 0 V

  Serial.begin(9600);

  pinMode(rxPin, INPUT);  // pins serial interface
  pinMode(txPin, OUTPUT);
  Serial1.begin(9600);

  test(2000);  // visible start, also wait for soft serial

  while (Serial.available())  // empty queues
    char c = Serial.read();
  while (Serial1.available())
    char c = Serial1.read();
  cmdUSBPtr = 0;
  cmdSerPtr = 0;

  Serial.println(F(HELP_DSC));
  Serial.println("ok");
  Serial1.println(F(HELP_DSC));
  Serial1.println("ok");
}


void loop() {

  // read from usb interface
  if (Serial.available()) {  // read from interface

    char c = Serial.read();
    // if (echo)
    //   Serial.print(c);

    if ((c == '\n') || (c == '\r')) {  // NL or CR
      cmdUSB[cmdUSBPtr] = 0;
      processCommand(cmdUSB);
      cmdUSBPtr = 0;  // start new command line
    } else {
      cmdUSB[cmdUSBPtr] = c;
      cmdUSBPtr++;
      cmdUSBPtr = cmdUSBPtr & 0x0F;
    }
  }

  // read from additional serial interface
  if (Serial1.available()) {  // read from interface

    char c = Serial1.read();
    if (echo) {
      Serial.print(cmdSerPtr);
      Serial.print(c);
      Serial.print(" ");
    }

    if ((c == '\n') || (c == '\r')) {  // NL or CR
      cmdSer[cmdSerPtr] = 0;
      processCommand(cmdSer);
      cmdSerPtr = 0;  // start new command line
    } else {
      cmdSer[cmdSerPtr] = c;
      cmdSerPtr++;
      cmdSerPtr = cmdSerPtr & 0x0F;
    }
  }

  // toogle LED in main loop
  if (millis() - previousMillis >= 1000) {  // every second
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    previousMillis = millis();
  }
}


void processCommand(char command[]) {

  if (command[0] == 0) {  // empty, from CR or NL
    return;
  }

  for (int i = 0; command[i] != '\0'; ++i) {
    if (command[i] >= 'A' && command[i] <= 'Z') {  // to lower case
      command[i] = command[i] + 'a' - 'A';
    }
  }

  if (debug) {
    Serial.print("cmd:");
    Serial.print(command);
    for (int i = 0; i < 16; i++) {  // debug: show command line
      Serial.print(" ");
      Serial.print(command[i], HEX);
    }
    Serial.println();
  }

  bool err = false;

  if (strcmp(command, "test") == 0) {
    test(1000);
  } else if (strcmp(command, "echo") == 0) {
    echo = !echo;
  } else if (strcmp(command, "debug") == 0) {
    debug = !debug;
  } else if ((strcmp(command, "info") == 0) || (command[0] == '?')) {

    Serial.println(F(HELP_DSC));
    Serial.print("status: echo=");
    Serial.print(echo);
    Serial.print("   debug=");
    Serial.println(debug);
    Serial.println(F(HELP_CMD));

    Serial1.println(F(HELP_DSC));
    Serial1.print("status: echo=");
    Serial1.print(echo);
    Serial1.print("   debug=");
    Serial1.println(debug);
    Serial1.println(F(HELP_CMD));

  } else if (strcmp(command, "reset") == 0) {
    resetFunc();                            //call reset
  } else if (strcmp(command, "ok") == 0) {  // nothing, just reply "ok"
    err = false;
  } else if (strcmp(command, "err") == 0) {  // nothing, just error code
    err = true;

  } else if (strcmp(command, "red") == 0) {
    digitalWrite(LED_R, ON);
    digitalWrite(LED_G, OFF);
    digitalWrite(LED_B, OFF);
  } else if (strcmp(command, "green") == 0) {
    digitalWrite(LED_R, OFF);
    digitalWrite(LED_G, ON);
    digitalWrite(LED_B, OFF);
  } else if (strcmp(command, "blue") == 0) {
    digitalWrite(LED_R, OFF);
    digitalWrite(LED_G, OFF);
    digitalWrite(LED_B, ON);
  } else if (strcmp(command, "yellow") == 0) {
    digitalWrite(LED_R, ON);
    digitalWrite(LED_G, ON);
    digitalWrite(LED_B, OFF);
  } else if (strcmp(command, "magenta") == 0) {
    digitalWrite(LED_R, ON);
    digitalWrite(LED_G, OFF);
    digitalWrite(LED_B, ON);
  } else if (strcmp(command, "cyan") == 0) {
    digitalWrite(LED_R, OFF);
    digitalWrite(LED_G, ON);
    digitalWrite(LED_B, ON);
  } else if (strcmp(command, "white") == 0) {
    digitalWrite(LED_R, ON);
    digitalWrite(LED_G, ON);
    digitalWrite(LED_B, ON);
  } else if (strcmp(command, "black") == 0) {
    digitalWrite(LED_R, OFF);
    digitalWrite(LED_G, OFF);
    digitalWrite(LED_B, OFF);
  } else if (strncmp(command, "out ", 4) == 0) {
    analogOut(command);
  } else if (strcmp(command, "in") == 0) {
    analogIn();
  } else if (strcmp(command, "loop") == 0) {
    analogLoop();
  } else err = true;

  if (err) {
    Serial.println("err");
    Serial1.println("err");
  } else {
    Serial.println("ok");
    Serial1.println("ok");
  }
}
