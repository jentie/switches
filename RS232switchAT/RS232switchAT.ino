/*
  RS232switchAT - switch controlled by AT commands via RS232 

  2024-06-01, Jens

  using ATcommands library - https://github.com/yourapiexpert/ATCommands
  based on example ATCommandsParameters.ino 
 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ATCommands.h>

#define OUT1 2
#define OUT2 3
#define OUT3 4
#define OUT4 5

#define IN1 6
#define IN2 7
#define IN3 8
#define IN4 9

#define AIN A0
#define AOUT 11

#define WORKING_BUFFER_SIZE 255  // The size of the working buffer (ie: the expected length of the input string)

ATCommands AT;  // create an instance of the class
String command;
String param1;
String param2;

int inputPin = 0;

int analogValue = 0;

String text = "";

bool blinkEnable = true;
unsigned long halfPeriod = 500;  // on & off time
unsigned long previousMillis;    // previous millis counter


//
//   AT+INFO - print help / info
//

bool at_run_cmd_info(ATCommands *sender) {
  Serial.println(F("RS232 Switch"));
  Serial.println(F("AT+<CMD> with"));
  Serial.println(F("commands: +INFO, +LED, +PER, +OUT, +IN, +ANA, +PRINT"));
  return true;
}


//
//  AT+PER - set blink period in ms
//

// AT+PER=<TIME>
bool at_write_cmd_per(ATCommands *sender) {

  param1 = sender->next();
  if ((param1.length() > 0) && (sender->next() == NULL)) {

    unsigned long period = param1.toInt();

    if (period) {
      halfPeriod = period / 2;
      return true;
    }
  }
  return false;
}

// AT+PER=? - show internal state
bool at_test_cmd_per(ATCommands *sender) {

  Serial.print("period: ");
  Serial.print(halfPeriod * 2);
  Serial.print(" ms or ");
  Serial.print((float)halfPeriod / 500);
  Serial.println(" s");
  Serial.print("LED blinking is ");
  if (blinkEnable)
    Serial.println("ON");
  else
    Serial.println("OFF");

  return true;
}


//
//  AT+LED - set LED state
//

// AT+LED=ON|OFF|BLINK|NOBLINK - write LED
bool at_write_cmd_led(ATCommands *sender) {

  param1 = sender->next();

  if ((param1.length() > 0) && (sender->next() == NULL)) {

    if (param1 == "ON")
      digitalWrite(LED_BUILTIN, HIGH);
    else if (param1 == "OFF")
      digitalWrite(LED_BUILTIN, LOW);
    else if (param1 == "BLINK")
      blinkEnable = true;
    else if (param1 == "NOBLINK")
      blinkEnable = false;
    else
      return false;

    return true;
  }
  return false;
}

// AT+LED=? - show internal state
bool at_test_cmd_led(ATCommands *sender) {

  if (blinkEnable)
    Serial.println("LES is blinking");
  else {
    if (digitalRead(LED_BUILTIN))
      Serial.println("LED is ON");
    else
      Serial.println("LED is OFF");
  }
  return true;
}

// AT+LED? - show LED state
bool at_read_cmd_led(ATCommands *sender) {

  command = sender->next();

  if (sender->next() == NULL) {
    if (digitalRead(LED_BUILTIN))
      Serial.println("ON");
    else
      Serial.println("OFF");

    return true;
  }
  return false;
}


void print_state(char *prefix, int num, int pin) {

  Serial.print(prefix);
  Serial.print(num);
  Serial.print(" is ");
  if (digitalRead(pin))
    Serial.println("ON");
  else
    Serial.println("OFF");
}


//
//  AT+OUT - set OUTPUT pin state
//

// AT+OUT=<num>,ON|OFF - write OUTPUT pin
bool at_write_cmd_out(ATCommands *sender) {

  bool state;

  param1 = sender->next();
  param2 = sender->next();

  if ((param1.length() > 0) && (param2.length() > 0) && (sender->next() == NULL)) {

    if (param2 == "ON")
      state = HIGH;
    else if (param2 == "OFF")
      state = LOW;
    else
      return false;

    int output = param1.toInt();

    switch (output) {
      case 1:
        digitalWrite(OUT1, state);
        break;
      case 2:
        digitalWrite(OUT2, state);
        break;
      case 3:
        digitalWrite(OUT3, state);
        break;
      case 4:
        digitalWrite(OUT4, state);
        break;
      case 9:
        digitalWrite(LED_BUILTIN, state);
        break;
      default:
        return false;
    }

    return true;
  }
  return false;
}

// AT+OUT=? - show all OUTPUT pin states
bool at_test_cmd_out(ATCommands *sender) {

  command = sender->next();

  if (sender->next() == NULL) {

    print_state("Out", 1, OUT1);
    print_state("Out", 2, OUT2);
    print_state("Out", 3, OUT3);
    print_state("Out", 4, OUT4);

    print_state("LED", 9, LED_BUILTIN);
    return true;
  }
  return false;
}


//
//  AT+IN - read INPUT pin state
//

// AT+IN=<num> - read input pin
bool at_write_cmd_in(ATCommands *sender) {

  bool state;

  param1 = sender->next();

  if ((param1.length() > 0) && (sender->next() == NULL)) {

    inputPin = param1.toInt();

    switch (inputPin) {
      case 1:
        state = digitalRead(IN1);
        break;
      case 2:
        state = digitalRead(IN2);
        break;
      case 3:
        state = digitalRead(IN3);
        break;
      case 4:
        state = digitalRead(IN4);
        break;
      case 9:
        state = digitalRead(LED_BUILTIN);
        break;
      default:
        return false;
    }
    if (state)
      Serial.println("ON");
    else
      Serial.println("OFF");

    return true;
  }
  return false;
}


// AT+IN? - show INPUT pin state
bool at_read_cmd_in(ATCommands *sender) {

  command = sender->next();

  if (sender->next() == NULL)
    if ((inputPin > 0) && (inputPin < 99)) {
      if (digitalRead(inputPin))
        Serial.println("ON");
      else
        Serial.println("OFF");
      return true;
    }
  return false;
}


// AT+IN=? - show all INPUT pin states
bool at_test_cmd_in(ATCommands *sender) {

  command = sender->next();

  if (sender->next() == NULL) {

    print_state("In", 1, OUT1);
    print_state("In", 2, OUT2);
    print_state("In", 3, OUT3);
    print_state("In", 4, OUT4);

    print_state("LED", 9, LED_BUILTIN);
    return true;
  }
  return false;
}


//
//  AT+ANA - Analog In / Out
//

// AT+ANA=<value> - write analog value
bool at_write_cmd_ana(ATCommands *sender) {

  param1 = sender->next();

  if ((param1.length() > 0) && (sender->next() == NULL)) {

    analogValue = param1.toInt();

    analogWrite(AIN, analogValue);

    return true;
  }
  return false;
}


// AT+ANA? - read analog value
bool at_read_cmd_ana(ATCommands *sender) {

  int value;

  command = sender->next();

  if (sender->next() == NULL) {

    value = analogRead(AIN);
    Serial.println(value);

    return true;
  }
  return false;
}


// AT+ANA=? - read and write analog value
bool at_test_cmd_ana(ATCommands *sender) {

  command = sender->next();

  if (sender->next() == NULL) {

    analogValue = analogRead(AIN);
    analogWrite(AIN, analogValue);

    Serial.println(analogValue);

    return true;
  }
  return false;
}


//
//   AT+PRINT - example command
//

// AT+PRINT=? - help and show internal state
bool at_test_cmd_print(ATCommands *sender) {
  sender->serial->print(sender->command);
  Serial.println(F("=<TEXT>"));
  Serial.print("current text: ");
  Serial.println(text);
  return true;
}

// AT+PRINT=param1 - set text
bool at_write_cmd_print(ATCommands *sender) {
  text = sender->next();
  return true;
}

// AT+PRINT - print text
bool at_run_cmd_print(ATCommands *sender) {
  if (text.length() > 0) {
    sender->serial->println(text);
    return true;  // tells ATCommands to print OK
  }
  return false;  // tells ATCommands to print ERROR
}


// AT commands
static at_command_t commands[] = {
  { "+INFO", at_run_cmd_info, NULL, NULL, NULL },
  { "+PER", NULL, at_test_cmd_per, NULL, at_write_cmd_per },
  { "+LED", NULL, at_test_cmd_led, at_read_cmd_led, at_write_cmd_led },
  { "+OUT", NULL, at_test_cmd_out, NULL, at_write_cmd_out },
  { "+IN", NULL, at_test_cmd_in, at_read_cmd_in, at_write_cmd_in },
  { "+ANA", NULL, at_test_cmd_ana, at_read_cmd_ana, at_write_cmd_ana },
  { "+PRINT", at_run_cmd_print, at_test_cmd_print, NULL, at_write_cmd_print },
};


void setup() {

  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, LOW);

  pinMode(IN1, INPUT);
  pinMode(IN2, INPUT);
  pinMode(IN3, INPUT);
  pinMode(IN4, INPUT);

  pinMode(AOUT, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  previousMillis = millis();

  Serial.begin(115200);

  Serial.println("\n\nRS232switchAT - Switch controlled by AT commands via RS232");
  Serial.println("OK");

  AT.begin(&Serial, commands, sizeof(commands), WORKING_BUFFER_SIZE);
}


void loop() {

  AT.update();

  // toogle / blink LED
  if (blinkEnable && (millis() - previousMillis >= halfPeriod)) {  // every second
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    previousMillis = millis();
  }
}
