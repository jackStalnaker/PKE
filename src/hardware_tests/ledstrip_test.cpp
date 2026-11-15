#include <Arduino.h>
#include <FastLED.h>
#include "ledstrip.h"

LEDStrip led_strip;

void setup() {
  led_strip.setup();
}

void loop() {
  led_strip.play();
}