/*
  nano-4digit-ser - 4 digits with serial interface

  20240324, Jens

  control small number display via ASCII commands

  notes:
  * TM1637 pins: CLK - D2 & DIO - D3
  * serial interfaces: USB & Tx/D6 / Rx/D7

  notes on Nano clone programming:
  * install CH340 driver?
  * select old bootloader?

  todo:
  * display analog value, esp. loop
  * set brightness?
  * special formats, e.g. float (-9.99 ... 9.99), time (0:00...23:59), temp (-99.9°...99.9°) 

*/

#include <SoftwareSerial.h>
#include <TM1637Display.h>

#define txPin 6
#define rxPin 7

#define ON HIGH
#define OFF LOW

#define CLK 2
#define DIO 3

#define A_IN A0
#define A_OUT 9

unsigned long previousMillis;  // variable for comparing millis counter

char command[16];  // command line
byte cptr = 0;

long lbound = 0;
long hbound = 9999;
bool hex = false;


bool echo = false;
bool debug = false;  // note: debug messages only via USB
// bool debug = true;

SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);  // add. serial i/f

TM1637Display display(CLK, DIO);

const uint8_t full[] = { 0xff, 0xff, 0xff, 0xff };
const uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
const uint8_t upper[] = { 0x01, 0x01, 0x01, 0x01 };
const uint8_t lower[] = { 0x08, 0x08, 0x08, 0x08 };


void (*resetFunc)(void) = 0;  //declare reset function @ address 0


void test(int dtime) {

  display.setSegments(blank);
  delay(dtime);
  display.setSegments(full);
  delay(dtime);
  display.setSegments(blank);
}


void pattern() {

  uint8_t data[4];

  display.setSegments(upper);
  delay(200);

  for (int n = 0; n < 5; n++) {
    byte s = 1;
    for (int m = 0; m < 6; m++) {
      data[0] = s;
      data[1] = s;
      data[2] = s;
      data[3] = s;
      display.setSegments(data);
      delay(20);
      s = s << 1;
    }
  }
  display.setSegments(blank);
}


void analogIn() {

  long value = analogRead(A_IN);

  Serial.print(value);
  Serial.print(" - 0x");
  Serial.println(value, HEX);

  mySerial.print(value);
  mySerial.print(" - 0x");
  mySerial.println(value, HEX);
}


void analogLoop() {

  while (true) {  // run forever

    int value = analogRead(A_IN);

    Serial.print(value);
    Serial.print(" - 0x");
    Serial.println(value, HEX);

    mySerial.print(value);
    mySerial.print(" - 0x");
    mySerial.println(value, HEX);

    delay(1000);  // every second

    if (Serial.available()) {  // stop with "any key"
      char c = Serial.read();
      if (c == 'q')
        break;
    }

    if (mySerial.available()) {  // stop with "any key"
      char c = mySerial.read();
      if (c == 'q')
        break;
    }
  }  // while
}


void analogOut() {
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

  analogWrite(A_OUT, 0);  // default: 0 V

  pinMode(rxPin, INPUT);  // pins serial interface
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);

  Serial.begin(9600);

  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  display.setBrightness(5);  // 0...7

  test(250);

  Serial.println("\nnano-4digit-ser - 4 digits with serial interface");
  Serial.println("ok");
  mySerial.println("\nnano-4digit-ser - 4 digits with serial interface");
  mySerial.println("ok");
}


void loop() {

  // read from usb interface
  if (Serial.available()) {  // read from interface

    char c = Serial.read();
    if (echo)
      Serial.print(c);

    if ((c == '\n') || (c == '\r')) {  // NL or CR
      if (echo)
        Serial.println();
      command[cptr] = 0;
      processCommand();
      cptr = 0;  // delete command line
    } else {
      command[cptr] = c;
      cptr++;
      cptr = cptr & 0x0F;
    }
  }

  // read from additional serial interface
  if (mySerial.available()) {  // read from interface

    char c = mySerial.read();

    if (echo)
      mySerial.print(c);

    if ((c == '\n') || (c == '\r')) {  // NL or CR
      if (echo)
        mySerial.println();
      command[cptr] = 0;
      processCommand();
      cptr = 0;  // delete command line
    } else {
      command[cptr] = c;
      cptr++;
      cptr = cptr & 0x0F;
    }
  }

  // toogle LED in main loop
  if (millis() - previousMillis >= 1000) {  // every second
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    previousMillis = millis();
  }
}


void processCommand() {

  if (command[0] == 0)  // empty, from CR or NL
    return;

  for (int i = 0; command[i] != '\0'; ++i) {
    if (command[i] >= 'A' && command[i] <= 'Z') {  // to lower case
      command[i] = command[i] - 'A' + 'a';
    }
  }

  if (debug) {
    Serial.println(command);
    for (int i = 0; i < 16; i++) {  // debug: show command line
      Serial.print(command[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  bool err = false;

  if (strcmp(command, "test") == 0) {
    test(250);
  } else if (strcmp(command, "pattern") == 0) {
    pattern();
  } else if (strcmp(command, "clear") == 0) {
    display.setSegments(blank);
  } else if (strcmp(command, "echo") == 0) {
    echo = !echo;
  } else if (strcmp(command, "debug") == 0) {
    debug = !debug;
  } else if ((strcmp(command, "info") == 0) || (command[0] == '?')) {

    Serial.println("nano-4digit-ser - 4 digits with serial interface");
    Serial.print("version 20240324   ");
    Serial.print("   echo: ");
    Serial.print(echo);
    Serial.print("   debug: ");
    Serial.println(debug);
    Serial.println("commands: test, echo, debug, info, reset");
    Serial.println("analog: out <num> (0...1023), in, loop (stop with 'q')");
    Serial.print("numbers: int, dec, hex - current range: ");
    Serial.print(lbound);
    Serial.print(" ... ");
    Serial.println(hbound);

    mySerial.println("nano-4digit-ser - 4 digits with serial interface");
    mySerial.print("version 2024034   ");
    mySerial.print("   echo: ");
    mySerial.print(echo);
    mySerial.print("   debug: ");
    mySerial.println(debug);
    mySerial.println("commands: test, echo, debug, info, reset");
    mySerial.println("analog: out <num> (0...1023), in, loop (stop with 'q')");
    mySerial.print("numbers: int, dec, hex - current range: ");
    mySerial.print(lbound);
    mySerial.print(" ... ");
    mySerial.println(hbound);

  } else if (strncmp(command, "out ", 4) == 0) {
    analogOut();
  } else if (strcmp(command, "in") == 0) {
    analogIn();
  } else if (strcmp(command, "loop") == 0) {
    analogLoop();

  } else if ((command[0] == 'b') && (command[2] == 0)) {
    int num = command[1] - '0';
    if (num < 0)
      num = 0;
    else if (num > 7)
      num = 7;
    display.setBrightness(num);  // applied with next output
  } else if (strcmp(command, "int") == 0) {
    lbound = 0;
    hbound = 9999;
    hex = false;
    display.showNumberDec(0, false);  // number zero
  } else if (strcmp(command, "dec") == 0) {
    lbound = -999;
    hbound = 999;
    hex = false;
    display.showNumberDec(0, false);  // number zero
  } else if (strcmp(command, "hex") == 0) {
    lbound = 0x0000;
    hbound = 0xffff;
    hex = true;
    display.showNumberHexEx(0x0000, 0, true);  // hex with trailing zeros
  } else {
    // if not command --> value

    long value = 0;

    if (hex) {
      int n, m;  // positions of hex digits in string
      if ((command[0] == '0') && (command[1] == 'x'))
        n = 2;
      else
        n = 0;
      m = n + 3;  // max. 4 digits

      while (true) {
        if (command[n] == 0)
          break;
        value = value << 4;

        int digit = command[n];
        if (digit >= 'a')
          digit = digit - 'a' + 10;  // string is in lower case
        else
          digit = digit - '0';

        if ((digit < 0) || (digit > 15)) {
          err = true;
          value = -1;  // todo: show error pattern
          break;
        }

        value = value + digit;

        n++;
        if (n > m)
          break;
      }

      if (value < lbound) {
        display.setSegments(lower);
        err = true;
      } else if (value > hbound) {
        display.setSegments(upper);
        err = true;
      } else
        display.showNumberHexEx(value, 0, true);  // trailing zeros

    } else {

      String number = command;

      value = number.toInt();

      if (value < lbound) {
        display.setSegments(lower);
        err = true;
      } else if (value > hbound) {
        display.setSegments(upper);
        err = true;
      } else
        display.showNumberDec(value, false);
    }

    if (debug) {
      Serial.print(command);
      Serial.print(" --> ");
      Serial.println(value);
    }

    if (err) {
      Serial.println("err");
      mySerial.println("err");
    } else {
      Serial.println("ok");
      mySerial.println("ok");
    }
  }
}
