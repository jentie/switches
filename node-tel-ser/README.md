# node-tel-ser - telnet serial bridge
  
### board: NodeMCU 0.9 

### notes
  * log info via USB serial interface @ 115200 
  * serial interface on TXD2 / D8 & RXD2 / D7
  * select internal loopback with D5 ( D5=hi --> loopback)
  * select baud rate with D2 & D1
      D2=hi & D1=hi --> 119200
      D2=hi & D1=lo -->  57600
      D2=lo & D1=ho -->  19200
      D2=lo & D1=lo -->   9600


### based on 8266 example
  
## WiFiTelnetToSerial - Example Transparent UART to Telnet Server for esp8266
  
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
