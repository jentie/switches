/*
  nano-rgb-ser - RGB LED with serial interface

  20240322, Jens

  note:
  * led pins: D2, D3, D4
  * serial interface: Tx/D1, Rx/D0

  notes on Nano clone programming:
  * install CH340 driver?
  * select old bootloader?

*/

#define ON HIGH
#define OFF LOW

#define LED_R 2
#define LED_G 3
#define LED_B 4

unsigned long previousMillis;  // variable for comparing millis counter

char command[16];  // command line
byte cptr = 0;

bool echo = false;
bool debug = false;


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

  Serial.begin(9600);

  test(250);

  Serial.println("\nnano-rgb-ser - RGB LED with serial interface");
  Serial.println("ok");
}


void loop() {

  while (Serial.available()) {  // read from serial interface

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
  } else if (strcmp(command, "info") == 0) {
    Serial.println("nano-rgb-ser - RGB LED with serial interface");
    Serial.print("version: 20240322");
    Serial.print("   echo: ");
    Serial.print(echo);
    Serial.print("   debug: ");
    Serial.println(debug);
    Serial.println("commands: test, echo, debug, info");
    Serial.println("colors: red, green, blue, yellow, magenta, cyan, white, black");
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
    digitalWrite(LED_B, ON);
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
  } else err = true;

  if (err)
    Serial.println("err");
  else
    Serial.println("ok");
}
