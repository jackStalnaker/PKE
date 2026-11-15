#include <Arduino.h>
#include "splashpage.h"
#include "readypage.h"

LGFX display;
SplashPage splash_page(display);
ReadyPage ready_page(display);

void setup() {
    display.init();
    display.setRotation(2);
    display.fillScreen(TFT_BLACK);
    splash_page.draw();
    ready_page.draw();
}

void loop() {

}