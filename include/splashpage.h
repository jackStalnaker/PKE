#include <Arduino.h>
#include "display.h"
#include "basepage.h"
#include "customcolors.h"
#include "assets/gblogo_color.h"
#include "assets/gbtext.h"

const char* VERSION = {"2.0"};

/*
 * Startup splash page
 */
class SplashPage : public BasePage {
  public:
    SplashPage(lgfx::LovyanGFX& display)
        : BasePage(display) {}

    void draw() {
      display_.fillScreen(TFT_BLACK);
      display_.setSwapBytes(true);
      display_.pushImage(32, 20, 256, 256, gblogo_color);
      display_.setSwapBytes(false);
      display_.drawBitmap(32, 290, gbtext, 256, 30, TFT_BLACK, TFT_WHITE);

      display_.setFont(&fonts::Orbitron_Light_24);
      display_.setTextSize(1);
      char buffer[12];
      snprintf(buffer, sizeof(buffer), "PKE v%s", VERSION);
      display_.setTextColor(TFT_WHITE, TFT_BLACK);
      display_.drawCenterString(buffer, 160, 330);
      display_.setFont(&fonts::Satisfy_24);
      display_.setTextSize(0.9);
      display_.drawCenterString("We ain't afraid of no ghosts!", 160, 380);
      display_.setFont(&fonts::AsciiFont8x16);
      display_.setTextSize(1);
      display_.drawString("Initializing ...", 10, 460);
      delay(500);
      display_.fillRect(0, 440, 320, 40, TFT_BLACK);
      display_.drawString("Compensating for ectoplasm ...", 10, 460);
      delay(500);
      display_.fillRect(0, 440, 320, 40, TFT_BLACK);
      display_.drawString("Accelerating Zeddmore particles ...", 10, 460);
      delay(500);
      display_.fillRect(0, 440, 320, 40, TFT_BLACK);
      display_.drawString("If found call (713) 555-2368", 10, 460);
      delay(1500);
    }
};