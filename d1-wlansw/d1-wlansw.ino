/*

  d1-wlansw - D1 mini based WLAN Switch

  2024-03-03, Jens

  LOLIN D1 mini
  https://www.wemos.cc/en/latest/d1/d1_mini.html

  set hostname from
  Rui Santos at https://RandomNerdTutorials.com/esp8266-nodemcu-set-custom-hostname-arduino/
  

  pinout:

     _________________
    |  |   /\  /\  |  |
    |  |  /  \/  \ |  |
RST |o |-----------| o| TX
 A0 |o |           | o| RX
 D0 |o |   ESP     | o| D1
 D5 |o |   8266    | o| D2
 D6 |o |           | o| D3
 D7 |o |           | o| D4
 D8 |o  -----------  o| GND
3V3 |o               o| 5V
    \     .......     | 
     |    { USB }     |
     +----+-----+-----+

*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// WLAN network credentials
#include "credentials.h"

String newHostname = "WLANSW-1";

// Create a UDP instance
WiFiUDP udp;

int verbose = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // D4
  digitalWrite(LED_BUILTIN, HIGH);  // off
  delay(1000);

  digitalWrite(LED_BUILTIN, LOW);  // on
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);  // on
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);  // on
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(D0, OUTPUT);  // four outputs ...
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  digitalWrite(D0, LOW);  // ... initally "off"
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  //Get Current Hostname
  Serial.printf("Default hostname: %s\n", WiFi.hostname().c_str());

  //Set new hostname
  WiFi.hostname(newHostname.c_str());

  //Get Current Hostname
  Serial.printf("New hostname: %s\n", WiFi.hostname().c_str());

  //Init Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  // Start UDP server
  udp.begin(23);  // telnet port
  Serial.print("UDP server started on port ");
  Serial.println(udp.localPort());
}


void reply_ok() {
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write("ok");
  udp.endPacket();
}


void reply_err() {
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write("err");
  udp.endPacket();
}


void loop() {
  // Check for incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // Read the packet data
    char packetData[packetSize];
    udp.read(packetData, packetSize);

    // terminate string from packet data
    packetData[packetSize] = 0;

    if (verbose) {
      int i = 0;
      while (packetData[i] != 0) {
        Serial.print(packetData[i], HEX);
        Serial.print(" ");
        i++;
      }
      Serial.print(" - ");
    }

    switch (packetData[0]) {
      case '?':
        if (verbose) {
          Serial.println("received: ? --> WLANSW-1");
          Serial.println("WLANSW-1 - commands: ? 0 1 R r G g B b W v V v");
        }
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        udp.write("WLANSW-1 ok");
        udp.endPacket();        
        break;
      case '1':
        digitalWrite(LED_BUILTIN, LOW);  // Turn on LED
        Serial.println("received: 1 (LED ON)");
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        udp.write("ok");
        udp.endPacket();
        break;
      case '0':
        digitalWrite(LED_BUILTIN, HIGH);  // Turn off LED
        Serial.println("received: 0 (LED OFF)");
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        udp.write("ok");
        udp.endPacket();
        break;

      case 'V':
        verbose++;
        Serial.println("received: verbose");
        reply_ok();
        break;
      case 'v':
        verbose = 0;
        Serial.println("received: quiet");
        reply_ok();
        break;

      case 'R':
        if (verbose)
          Serial.println("received: R on");
        digitalWrite(D0, HIGH);
        reply_ok();
        break;
      case 'r':
        if (verbose)
          Serial.println("received: R off");
        digitalWrite(D0, LOW);
        reply_ok();
        break;

      case 'G':
        if (verbose)
          Serial.println("received: G on");
        digitalWrite(D1, HIGH);
        reply_ok();
        break;
      case 'g':
        if (verbose)
          Serial.println("received: g off");
        digitalWrite(D1, LOW);
        reply_ok();
        break;

      case 'B':
        if (verbose)
          Serial.println("received: B on");
        digitalWrite(D2, HIGH);
        reply_ok();
        break;
      case 'b':
        if (verbose)
          Serial.println("received: b off");
        digitalWrite(D2, LOW);
        reply_ok();
        break;

      case 'W':
        if (verbose)
          Serial.println("received: W on");
        digitalWrite(D3, HIGH);
        reply_ok();
        break;
      case 'w':
        if (verbose)
          Serial.println("received: w off");
        digitalWrite(D3, LOW);
        reply_ok();
        break;

      default:
        Serial.print("invalid packet received: ");
        Serial.println(packetData);
        reply_err();
    }
  }
}
