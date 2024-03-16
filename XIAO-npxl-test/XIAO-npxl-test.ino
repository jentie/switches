/*
  XIAO-npxl-test - test controlling some neopixels

  20240316, Jens

  note:
  two groups of 5V / 3W neopixel modules
  3 x GRB
  3 x RGBW 

*/

#include <Adafruit_NeoPixel.h>

#define PIN 1        // Pin, an dem die Neopixel angeschlossen sind
#define NUMPIXELS 3  // Anzahl der Neopixel in der Reihe

// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);



void setup() {
  pixels.begin();
  pixels.show();  // Initialisierung der Neopixel

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0, 0));
  }
  pixels.show();
  delay(1000);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0, 0));  // Rot (R, G, B)
  }
  pixels.show();
  delay(1000);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0, 0));  // Grün (R, G, B)
  }
  pixels.show();
  delay(1000);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255, 0));  // Grün (R, G, B)
  }
  pixels.show();
  delay(1000);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0, 255));  // Blau (R, G, B)
  }
  pixels.show();
  delay(2000);
}


void loop() {

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 255, 255));
  }
  pixels.show();
  delay(10000);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0, 255));
  }
  pixels.show();
  delay(10000);


  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(128, 255, 0, 255));
  }
  pixels.show();
  delay(10000);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255, 255));
  }
  pixels.show();
  delay(10000);
}
