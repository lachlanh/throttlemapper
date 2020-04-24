#include "Display.h"
#include <FastLED.h>

#define NUM_LEDS 4
#define VLED_PIN 10
CRGB leds[NUM_LEDS];

void setupDisplay () {
  FastLED.addLeds<WS2812, VLED_PIN, GRB>(leds, NUM_LEDS);
}

void updateDisplay(long cadence, long kph, float volts, float ah)
{
  //Serial.println(F("Free RAM = ")); //F function does the same and is now a built in library, in IDE > 1.0.0
  //Serial.println(freeMemory(), DEC);

  if (volts >= 40.0) {
    leds[0] = CRGB::Blue;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::Blue;
    leds[3] = CRGB::Blue;
  } else if (volts >= 38.0) {
    leds[0] = CRGB::Green;
    leds[1] = CRGB::Green;
    leds[2] = CRGB::Green;
    leds[3] = CRGB::Black;
  } else if (volts >= 36.0) {
    leds[0] = CRGB::White;
    leds[1] = CRGB::White;
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Black;
  } else if (volts > 0.0) {
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Black;
  } else {
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::White;
    leds[3] = CRGB::Green;
  }
  FastLED.show();

}
