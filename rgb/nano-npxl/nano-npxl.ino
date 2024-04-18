/*
  nano-npxl - control some neopixel with command via serial interface

  2024-03-13, Jens



*/



#include <Adafruit_NeoPixel.h>

#define PIN            6  // Pin where the WS2812 data line is connected
#define NUM_LEDS       6  // Number of LEDs in your strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();
  strip.show();  // Initialize all LEDs to 'off'
}

uint32_t getColorFromName(String colorName) {
  colorName.toLowerCase();  // Convert color name to lowercase
  if (colorName == "red") return strip.Color(255, 0, 0);
  if (colorName == "green") return strip.Color(0, 255, 0);
  if (colorName == "blue") return strip.Color(0, 0, 255);
  // Add more color mappings here as needed
  return 0;  // Default to off (black)
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');  // Read the input string until newline character
    input.trim();  // Remove leading/trailing spaces
    input.toLowerCase();   // LED --> led

    if (input.length() == 6) {  // Expecting a 6-character hex string (e.g., "FF00AA")
      uint32_t color = strtol(input.c_str(), NULL, 16);  // Convert hex string to 32-bit color value
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, color);  // Set color for each LED
      }
      strip.show();  // Update LED colors
      Serial.println("ok");  // Respond with "ok"
    } else if (input.startsWith("led ")) {
      int ledIndex = input.substring(4).toInt();  // Extract LED index
      if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
        input = Serial.readStringUntil('\n');  // Read the next line for color
        input.trim();
        uint32_t color = getColorFromName(input);  // Get color from standard name
        if (color != 0) {
          strip.setPixelColor(ledIndex, color);
          strip.show();
          Serial.println("ok");
        } else {
          Serial.println("Invalid color name. Available colors: red, green, blue, ...");
        }
      } else {
        Serial.println("Invalid LED index. Expected a value between 0 and 5.");
      }
    } else {
      Serial.println("Invalid input. Expected 6-character hex string or LED command.");
    }
  }
}
