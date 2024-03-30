/*
  DigiPro-rgb-ser - RGB LED with serial interface

  20240328, Jens

  control a RGB led via ASCII commands
  additional analog output and input

  notes:
  * led pins: D2, D3, D4
  * analog pins Ain-A12 & Aout-D0
  * only one serial interface (?), on Tx/Rx pins

  notes on Digispark programming:
  * insert USB after beginning of upload

*/


// Ardunio Nano
// #include <SoftwareSerial.h>
// #define txPin 6
// #define rxPin 7
// SoftwareSerial Serial1 = SoftwareSerial(rxPin, txPin);  // add. serial i/f
// #define A_IN A0
// #define A_OUT 0

// ATtiny167
#define A_IN A12
#define A_OUT 0 


#define ON HIGH
#define OFF LOW

#define LED_R 2
#define LED_G 3
#define LED_B 4

unsigned long previousMillis;  // variable for comparing millis counter

char command[16];  // command line
byte cptr = 0;

bool echo = false;
bool debug = false;   // note: debug messages only via USB


#define HELP_DSC "\nnano-rgb-ser - RGB LED with serial interface\nversion 20240323"
#define HELP_CMD "\
commands: test, echo, debug, info, reset\n\
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

  // Serial1.print(value);
  // Serial1.print(" - 0x");
  // Serial1.println(value, HEX);
}


void analogLoop() {

  while (true) {  // run forever

    int value = analogRead(A_IN);

    Serial.print(value);
    Serial.print(" - 0x");
    Serial.println(value, HEX);

    // Serial1.print(value);
    // Serial1.print(" - 0x");
    // Serial1.println(value, HEX);

    delay(1000);  // every second

    if (Serial.available()) {  // stop with "any key"
      char c = Serial.read();
      if (c == 'q')
        break;
    }

    // if (Serial1.available()) {  // stop with "any key"
    //   char c = Serial1.read();
    //   if (c == 'q')
    //     break;
    // }
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

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);  // Nano pins
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_R, OFF);
  digitalWrite(LED_G, OFF);  // default: OFF
  digitalWrite(LED_B, OFF);

  analogWrite(A_OUT, 0);  // default: 0 V

  // pinMode(rxPin, INPUT);  // pins serial interface
  // pinMode(txPin, OUTPUT);
  
  Serial.begin(9600);
  // Serial1.begin(9600);

  test(250);

  Serial.println(F(HELP_DSC));
  Serial.println("ok");
//   Serial1.println(F(HELP_DSC));
//   Serial1.println("ok");
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

  // // read from additional serial interface
  // if (Serial1.available()) {  // read from interface

  //   char c = Serial1.read();

  //   if (echo)
  //     Serial1.print(c);

  //   if ((c == '\n') || (c == '\r')) {  // NL or CR
  //     if (echo)
  //       Serial1.println();
  //     command[cptr] = 0;
  //     processCommand();
  //     cptr = 0;  // delete command line
  //   } else {
  //     command[cptr] = c;
  //     cptr++;
  //     cptr = cptr & 0x0F;
  //   }
  // }

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

    // Serial1.println(F(HELP_DSC));
    // Serial1.print("status: echo=");
    // Serial1.print(echo);
    // Serial1.print("   debug=");
    // Serial1.println(debug);
    // Serial1.println(F(HELP_CMD));
    
  } else if (strcmp(command, "reset") == 0) {
    resetFunc();  //call reset
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
    analogOut();
  } else if (strcmp(command, "in") == 0) {
    analogIn();
  } else if (strcmp(command, "loop") == 0) {
    analogLoop();
  } else err = true;

  if (err) {
    Serial.println("err");
    // Serial1.println("err");
  } else {
    Serial.println("ok");
    // Serial1.println("ok");
  }
}
