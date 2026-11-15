#include <Arduino.h>
#include "display.h"
#include "basepage.h"
#include "customcolors.h"
#include "assets/gblogo_color.h"
#include "assets/gbtext.h"

/*
 * Startup splash page
 */
class ReadyPage : public BasePage {
public:
    ReadyPage(lgfx::LovyanGFX& display)
        : BasePage(display) {}

    void draw() {
      switch (stage_) {
        case 0:
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
          display_.drawCenterString("Ready to scan", 160, 380);
          ++stage_;
          break;
        
        case 1:
          break;
      }
    }

    void reset() {
      stage_ = 0;
    }

private:
    int stage_ = 0;
};