#include "Display.h"
#include "Config.h"
//#include <FastLED.h>

#ifdef REPORT_MEMORY
  //#include <MemoryFree.h>
#endif

//CRGB leds[NUM_LEDS];

void setupDisplay () {
  //FastLED.addLeds<WS2812, VLED_PIN, GRB>(leds, NUM_LEDS);
}

void updateDisplay(long cadence, long kph, float volts, float ah)
{
  
  
/*
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
    //0.0 is sent when no connection to vesc, useful for debug
    leds[0] = CRGB::Red;
    leds[1] = CRGB::Blue;
    leds[2] = CRGB::White;
    leds[3] = CRGB::Green;
  }
  FastLED.show();
*/
}

void reportMemory () {
  #ifdef REPORT_MEMORY 
  //Serial.println(F("Free RAM = "));
  //Serial.println(freeMemory(), DEC);
  #endif
}
