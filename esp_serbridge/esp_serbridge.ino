/*
  esp-serbridge - telnet to serial bridge

  20240405, Jens


  todo:
  * kind of loopback/echo from external serial receiver to transmitter

  based on TelnetServerExample from Lennart Hennigs ESPTelnet
  https://github.com/LennartHennigs/ESPTelnet/blob/main/examples/TelnetServerExample/TelnetServerExample.ino

*/

#include "ESPTelnet.h"

#include "credentials.h"
// const char* ssid = "...";
// const char* password = "...";

#define MAX_SRV_CLIENTS 1  // number of telnet clients

#define LOGGER_SPEED 9600
#define SERIAL_SPEED 9600

const char* hostname = "SerBridge";


ESPTelnet telnet;
IPAddress ip;
uint16_t port = 23;

bool debug = true;


unsigned long previousMillis;  // variable for comparing millis counter
unsigned long statusToggle = 250;


// bool isConnected() {
//   return (WiFi.status() == WL_CONNECTED);
// }


bool connectToWiFi(int max_tries = 20) {
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.setHostname(hostname);  // set hostname in local network
  WiFi.hostname(hostname);     // set hostname in local network

  WiFi.begin(ssid, password);
  do {
    delay(500);
    Serial.print(".");
    i++;
  } while ((WiFi.status() != WL_CONNECTED) && (i < max_tries));
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  return (WiFi.status() == WL_CONNECTED);
}


void errorMsg(String error, bool restart = true) {
  Serial.println(error);
  if (restart) {
    Serial.println("rebooting now...");
    delay(2000);
    ESP.restart();
    delay(2000);
  }
}


void setupTelnet() {
  // passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  telnet.onInputReceived(onTelnetInput);

  Serial.print("telnet: ");
  if (telnet.begin(port)) {
    Serial.println("running");
  } else {
    Serial.println("error.");
    errorMsg("will reboot...");
  }
}


//
// (optional) callback functions for telnet events
//

void onTelnetConnect(String ip) {
  Serial.print("telnet: ");
  Serial.print(ip);
  Serial.println(" connected");

  telnet.println("\nWelcome " + telnet.getIP());
  telnet.println("(Use ^] + q  to disconnect.)");

  statusToggle = 1000;
}


void onTelnetDisconnect(String ip) {
  Serial.print("telnet: ");
  Serial.print(ip);
  Serial.println(" disconnected");

  statusToggle = 250;
}


void onTelnetReconnect(String ip) {
  Serial.print("telnet: ");
  Serial.print(ip);
  Serial.println(" reconnected");

  statusToggle = 1000;
}


void onTelnetConnectionAttempt(String ip) {
  Serial.print("telnet: ");
  Serial.print(ip);
  Serial.println(" tried to connect");
}


void onTelnetInput(String str) {

  if (debug) {
    Serial.print("-->");
    for (int i = 0; i < str.length(); i++)
      Serial.print(str.charAt(i), HEX);
    Serial.print("-->");
    Serial.print(str);
    Serial.println("-->");
  }

  Serial2.println(str);
}


void setup() {

  // status led: fast - idle / slow - telnet active
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  previousMillis = millis();
  statusToggle = 250;

  Serial.begin(LOGGER_SPEED);
  Serial.println("   ");
  Serial.println("\nesp-serbridge - telnet to serial bridge");
  Serial.println(ESP.getSdkVersion());

  Serial.print("wifi ");
  connectToWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    ip = WiFi.localIP();
    Serial.println();
    Serial.print("telnet server ");
    Serial.print(ip);
    Serial.print(" / ");
    Serial.print(WiFi.getHostname());
    Serial.println();
    setupTelnet();
  } else {
    Serial.println();
    errorMsg("error connecting to wifi");
  }

  Serial2.begin(SERIAL_SPEED);
}


char line[256];
byte lineptr = 0;


void loop() {

  telnet.loop();

  // send serial input to telnet connection
  if (Serial2.available()) {

    char c = Serial2.read();

    if ((c == '\n') || (c == '\r')) {  // NL or CR
      line[lineptr] = 0;
      telnet.println(line);

      if (debug) {
        Serial.print(">");
        Serial.print(line);
        Serial.println("<");
      }

      lineptr = 0;  // start new command line
    } else {
      line[lineptr] = c;
      lineptr++;
    }
  }

  // toogle LED in main loop
  if (millis() - previousMillis >= statusToggle) {  // every second
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    previousMillis = millis();
  }
}
