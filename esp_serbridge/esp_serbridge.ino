/*
  esp-serbridge - telnet to serial bridge

  20240401, Jens


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

const char* hostname = "SerBridge";

#define LOGGER_SPEED 9600
#define SERIAL_SPEED 9600

ESPTelnet telnet;
IPAddress ip;
uint16_t port = 23;


unsigned long previousMillis;  // variable for comparing millis counter
unsigned long statusToggle = 250;


/* ------------------------------------------------- */

bool isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}


bool connectToWiFi(int max_tries = 20, int pause = 500) {
  int i = 0;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.hostname(hostname);  // set hostname in local network

  WiFi.begin(ssid, password);
  do {
    delay(pause);
    Serial.print(".");
    i++;
  } while (!isConnected() && i < max_tries);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  return isConnected();
}


void errorMsg(String error, bool restart = true) {
  Serial.println(error);
  if (restart) {
    Serial.println("Rebooting now...");
    delay(2000);
    ESP.restart();
    delay(2000);
  }
}

/* ------------------------------------------------- */

void setupTelnet() {
  // passing on functions for various telnet events
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  telnet.onInputReceived(onTelnetInput);

  Serial.print("- Telnet: ");
  if (telnet.begin(port)) {
    Serial.println("running");
  } else {
    Serial.println("error.");
    errorMsg("Will reboot...");
  }
}

/* ------------------------------------------------- */

// (optional) callback functions for telnet events
void onTelnetConnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" connected");

  telnet.println("\nWelcome " + telnet.getIP());
  telnet.println("(Use ^] + q  to disconnect.)");

  Serial2.println("info");
}

void onTelnetDisconnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" disconnected");
}

void onTelnetReconnect(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" reconnected");
}

void onTelnetConnectionAttempt(String ip) {
  Serial.print("- Telnet: ");
  Serial.print(ip);
  Serial.println(" tried to connected");
}

void onTelnetInput(String str) {

  Serial.print("-->");
  for (int i = 0; i < str.length(); i++)
    Serial.print(str.charAt(i), HEX);
  Serial.print("-->");
  Serial.print(str);
  Serial.println("-->");

  Serial2.println(str);
}


void setup() {

  // status led: fast - idle / slow - telnet active
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  previousMillis = millis();
  statusToggle = 250;

  Serial.begin(LOGGER_SPEED);
  Serial.println("\n\n\esp-serbridge - telnet to serial bridge");
  Serial.println(ESP.getSdkVersion());

  Serial.print("wifi ");
  connectToWiFi();

  if (isConnected()) {
    ip = WiFi.localIP();
    Serial.println();
    Serial.print("telnet ");
    Serial.print(ip);
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
      Serial.print(">");
      Serial.print(line);
      Serial.println("<");
      lineptr = 0;  // start new command line
    } else {
      line[lineptr] = c;
      lineptr++;
    }
  }
}
