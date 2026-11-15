#include <Arduino.h>
#include "sound.h"

Sound sound;

void setup() {
  sound.setup();
}

void loop() {
  sound.play();
}