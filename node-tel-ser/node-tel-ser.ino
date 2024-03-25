/*
  node-tel-ser - telnet serial bridge

  20240324, Jens
  
  board: NodeMCU 0.9 

  notes:
  * log info via USB serial interface @ 115200 
  * serial interface on TXD2 / D8 & RXD2 / D7
  * select internal loopback with D5 ( high = loopback)
  * select baud rate with D2 & D1
      D2=hi & D1=hi --> 119200
      D2=hi & D1=lo -->  57600
      D2=lo & D1=ho -->  19200
      D2=lo & D1=lo -->   9600

  based on 8266 example:
  WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266
  https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiTelnetToSerial/WiFiTelnetToSerial.ino

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the ESP8266WiFi library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <ESP8266WiFi.h>
#include <esp8266_peri.h>

#include <algorithm>  // std::min

#include "credentials.h"

#define BAUD_SERIAL 115200
#define BAUD_LOGGER 115200
#define RXBUFFERSIZE 1024

////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>
SoftwareSerial* logger = nullptr;

#define STACK_PROTECTOR 512  // bytes

// how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2

bool loopback = false;


const int port = 23;

WiFiServer server(port);
WiFiClient serverClients[MAX_SRV_CLIENTS];


void setup() {

  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);

  long baud;
  if (digitalRead(D2))
    if (digitalRead(D1))
      baud = 119200;
    else
      baud = 57600;
  else if (digitalRead(D1))
    baud = 19200;
  else
    baud = 9600;

  Serial.begin(baud);
  Serial.setRxBufferSize(RXBUFFERSIZE);

  Serial.swap();  // hardware serial is now on RX:GPIO13 TX:GPIO15

  // use EspSoftwareSerial on regular RX(3)/TX(1) for logging
  logger = new SoftwareSerial(3, 1);
  logger->begin(BAUD_LOGGER);
  logger->enableIntTx(false);

  logger->println("\n\n\nnode-tel-ser - telnet serial bridge");
  logger->println(ESP.getFullVersion());
  logger->printf("serial baud rate: %d (8n1)\n", BAUD_SERIAL);
  logger->printf("serial receive buffer size: %d bytes\n", RXBUFFERSIZE);

  pinMode(D5, INPUT_PULLUP);
//  loopback = digitalRead(D5);

  if (loopback) {
    USC0(0) |= (1 << UCLBE);  // incomplete HardwareSerial API
    logger->println("internal loopback enabled");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  logger->print("\nConnecting to ");
  logger->println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    logger->print('.');
    delay(500);
  }
  logger->println();
  logger->print("connected, address=");
  logger->println(WiFi.localIP());

  // start server
  server.begin();
  server.setNoDelay(true);

  logger->print("Ready! Use 'telnet ");
  logger->print(WiFi.localIP());
  logger->printf(" %d' to connect\n", port);
}


void loop() {
  // check if there are any new clients
  if (server.hasClient()) {
    // find free/disconnected spot
    int i;
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
      if (!serverClients[i]) {  // equivalent to !serverClients[i].connected()
        serverClients[i] = server.accept();
        logger->print("new client: index ");
        logger->println(i);
        break;
      }

    // no free/disconnected spot so reject
    if (i == MAX_SRV_CLIENTS) {
      server.accept().println("busy");
      // hints: server.accept() is a WiFiClient with short-term scope
      // when out of scope, a WiFiClient will
      // - flush() - all data will be sent
      // - stop() - automatically too
      logger->printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
    }
  }

  // check TCP clients for data
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    while (serverClients[i].available() && Serial.availableForWrite() > 0) {
      // working char by char is not very efficient
      Serial.write(serverClients[i].read());
    }

  // determine maximum output size "fair TCP use"
  // client.availableForWrite() returns 0 when !client.connected()
  int maxToTcp = 0;
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    if (serverClients[i]) {
      int afw = serverClients[i].availableForWrite();
      if (afw) {
        if (!maxToTcp) {
          maxToTcp = afw;
        } else {
          maxToTcp = std::min(maxToTcp, afw);
        }
      } else {
        // warn but ignore congested clients
        logger->println("one client is congested");
      }
    }

  // check UART for data
  size_t len = std::min(Serial.available(), maxToTcp);
  len = std::min(len, (size_t)STACK_PROTECTOR);
  if (len) {
    uint8_t sbuf[len];
    int serial_got = Serial.readBytes(sbuf, len);
    // push UART data to all connected telnet clients
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
      // if client.availableForWrite() was 0 (congested)
      // and increased since then,
      // ensure write space is sufficient:
      if (serverClients[i].availableForWrite() >= serial_got) {
        size_t tcp_sent = serverClients[i].write(sbuf, serial_got);
        if (tcp_sent != len) { logger->printf("len mismatch: available:%zd serial-read:%zd tcp-write:%zd\n", len, serial_got, tcp_sent); }
      }
  }
}
