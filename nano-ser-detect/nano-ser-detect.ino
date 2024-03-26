/*
  nano-ser-detect - detect serial interface signals

  20240326, Jens


  notes:
  * signal input pins ....

  common baud rates
  baud rate   bit time   boundary
    (230400)        (4)         6
     115200          9         13
     (74880         13) 
      57600         17         21
      38400         26         30
     (31250         32)        
      28800         35         43
      19200         52         84
       9600        104        156
       4800        208        312
       2400        417        626
       1200        833       1250
        600       1667       2500 
        300       3333       4167
       (200)     (5000)         
     

  notes on Nano clone programming:
  * install CH340 driver?
  * select old bootloader?

*/

#include <TM1637Display.h>

#define CLK 18
#define DIO 19

#define IN2 2
#define IN3 3

bool debug = true;
//bool debug = false;


TM1637Display display(CLK, DIO);

const uint8_t full[] = { 0xff, 0xff, 0xff, 0xff };
const uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
const uint8_t minus[] = { 0x40, 0x40, 0x40, 0x40 };


void test(int dtime) {

  display.setSegments(blank);
  delay(dtime);
  display.setSegments(full);
  delay(dtime);
  display.setSegments(minus);
  delay(dtime);
  display.setSegments(blank);
}


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(IN2, INPUT_PULLUP);  // neutral / "start bit"
  pinMode(IN3, INPUT_PULLUP);

  pinMode(CLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  display.setBrightness(5);  // 0...7
  test(250);

  Serial.begin(115200);
  Serial.println("\n\nnano-ser-detect - detect serial interface signals");
}


long detectRate(int input) {

  unsigned long sig;
  unsigned long rate = 9999;
  long baud;

  for (int i = 0; i < 21; i++) {
    sig = pulseIn(input, LOW, 200000L);  // measure next zero period, timeout 0.2s

    if (debug) {
      Serial.print(sig);
      Serial.print("   ");
    }

    if ((sig > 0) && (sig < rate))
      rate = sig;

    delay(random(2, 10));  // different characters, no pattern
  }

  if (debug) {
    Serial.print(" shortest --> ");
    Serial.println(rate);
  }

  if (rate < 6)  // no pulse detected or too fast
    baud = 0;
  else if (rate < 13)
    baud = 115200;  // 9
  else if (rate < 21)
    baud = 57600;  // 17
  else if (rate < 30)
    baud = 38400;  // 26
  else if (rate < 43)
    baud = 28800;  // 35
  else if (rate < 84)
    baud = 19200;  // 52   ok
  else if (rate < 156)
    baud = 9600;  // 104   ok
  else if (rate < 312)
    baud = 4800;  // 208
  else if (rate < 626)
    baud = 2400;  // 417
  else if (rate < 1250)
    baud = 1200;  // 833
  else if (rate < 2500)
    baud = 600;  // 1667
  else if (rate < 4167)
    baud = 300;  // 3333   ok
  else baud = 0;

  return baud;
}

int baud2num(long baud) {

  switch (baud) {
    case 115200: return 11;
    case 57600: return 57;
    case 38400: return 38;
    case 28800: return 28;
    case 19200: return 19;
    case 9600: return 96;
    case 4800: return 48;
    case 2400: return 24;
    case 1200: return 12;
    case 600: return 60;
    case 300: return 30;
    default: return 88;
  }
}

void loop() {

  long baud2, baud3;

  digitalWrite(LED_BUILTIN, HIGH);  // start measurements

  baud2 = detectRate(IN2);  // start measurement (peripheral devices Tx)
  if (baud2) {
    Serial.print("Pin 2 - signal detect, baud rate ");
    Serial.println(baud2);
    display.showNumberDecEx(baud2num(baud2), 0xff, false, 2, 0);
  } else {
    Serial.println("no signal on Pin 2");
    display.setSegments(minus, 2, 0);
  }

  baud3 = detectRate(IN3);  // start measurement (computer Tx)
  if (baud3) {
    Serial.print("Pin 3 - signal detect, baud rate ");
    Serial.println(baud3);
    display.showNumberDecEx(baud2num(baud3), 0xff, false, 2, 2);
  } else {
    Serial.println("no signal on Pin 3");
    display.setSegments(minus, 2, 2);         // todo: set dots
  }

  digitalWrite(LED_BUILTIN, LOW);  // show display

  delay(3000);
}
