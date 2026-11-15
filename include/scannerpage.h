#pragma once
#include <Arduino.h>
#include "display.h"
#include "customcolors.h"
#include "basepage.h"
#include "tasktimer.h"

class ScannerPage : public BasePage {
  public:
    ScannerPage(lgfx::LovyanGFX& display)
        : BasePage(display), sprite_(&display) {}
        
    void make_scan_sprite(bool use_color_ghosts = false) {
      // check if sprite already exists
      if (!sprite_.width()) {
        if (use_color_ghosts) {
          sprite_.setColorDepth(4);
        } else {
          sprite_.setColorDepth(1);
        }

        sprite_.createSprite(grid_w_, grid_h_);

        if (use_color_ghosts) {
          sprite_.setPaletteColor(0, TFT_BLACK);
          sprite_.setPaletteColor(1, PHOSPHOR_CYAN);
          sprite_.setPaletteColor(2, TFT_GREEN);
          sprite_.setPaletteColor(3, TFT_RED);
          sprite_.setPaletteColor(4, TFT_YELLOW);
          sprite_.setPaletteColor(5, TFT_MAGENTA);
        } else {
          sprite_.setBitmapColor(PHOSPHOR_CYAN, TFT_BLACK);
        }
      }

      sprite_.fillSprite(0);  // erase previous sprite

      // Grid
      for (int y = 0; y < grid_h_; y += grid_dy_)
          sprite_.drawFastHLine(0, y, grid_w_, 1);

      for (int x = 0; x < grid_w_; x += grid_dx_)
          sprite_.drawFastVLine(x, 0, grid_h_, 1);

      // Fake ghost detections
      for (int i = 0; i < nghosts_; ++i)
        if (use_color_ghosts) {
          sprite_.fillCircle(random(20, grid_w_ - 20),
                            random(20, grid_h_ - 20),
                            random(10, ghost_r_),
                            random(1, 6));
        } else {
          sprite_.fillCircle(random(20, grid_w_ - 20),
                            random(20, grid_h_ - 20),
                            random(10, ghost_r_ + 1),
                            1);
        }

      offset_ = 0;
      timer_.reset();
    }

    void draw_UI(uint16_t bg_color = TFT_BLACK, uint16_t fg_color = PHOSPHOR_CYAN, uint16_t text_color = TFT_WHITE) {
      BasePage::draw_UI(bg_color, fg_color);
      display_.setFont(&fonts::AsciiFont8x16);
      display_.setTextColor(text_color);
      display_.setTextSize(1.5);
      display_.drawString(">> PKE 2.0 // SCANNING", 10, 10);
    }

    void draw() {
      switch (stage_) {
        case 0:
          display_.fillScreen(TFT_BLACK);
          draw_UI();
          make_scan_sprite(true);

          display_.setCursor(10, 300);
          display_.setTextSize(1.2);
          display_.setTextColor(PHOSPHOR_CYAN);
          display_.printf("SCANNING POWER (kW)");
          display_.setCursor(10, 360);
          display_.printf("PSYCHOKINETIC ENERGY (kJ)");
          display_.setCursor(300, 390);
          display_.printf("L");
          display_.setCursor(300, 425);
          display_.printf("R");
          draw_battery(275, 300, 30, 16, 75);
          ++stage_;

        case 1:
          if (!timer_.expired(50))
              return;

          timer_.reset();

          // Wrap the sprite horizontally
          sprite_.pushSprite(-offset_, 45);
          sprite_.pushSprite(grid_w_ - offset_, 45); 

          offset_ = (offset_ + d_offset_) % grid_w_;

          float power = random(5, 11) / 10.0f;
          draw_segment_bar(10, 330, 310, 20, 20, power, PHOSPHOR_AMBER);

          if (energy_timer_.expired(100)) {
            float energy_l = random(11) / 10.0f;
            float energy_r = random(11) / 10.0f;
            int schrod = random(11);

            if (energy_l <= 0.3) {
              draw_segment_bar(10, 390, 290, 20, 20, energy_l, PHOSPHOR_GREEN);
            } else if (energy_l <= 0.6) {
              draw_segment_bar(10, 390, 290, 20, 20, energy_l, PHOSPHOR_AMBER);
            } else {
              draw_segment_bar(10, 390, 290, 20, 20, energy_l, ALERT_RED);
            }
            if (energy_r <= 0.3) {
              draw_segment_bar(10, 420, 290, 20, 20, energy_r, PHOSPHOR_GREEN);
            } else if (energy_r <= 0.6) {
              draw_segment_bar(10, 420, 290, 20, 20, energy_r, PHOSPHOR_AMBER);
            } else {
              draw_segment_bar(10, 420, 290, 20, 20, energy_r, ALERT_RED);
            }

            display_.setCursor(10, 460);
            display_.setTextSize(1);
            display_.setTextColor(PHOSPHOR_CYAN);
            display_.fillRect(150, 460, 60, 15, TFT_BLACK);
            if (schrod <= 7) {
              display_.printf("ECTO: Y | SCHROD: =^._.^= | HEISEN: U");
            } else {
              display_.printf("ECTO: Y | SCHROD: =^x_x^= | HEISEN: U");
            }

            energy_timer_.reset();
          }
      }
    }

    void reset() {
      stage_ = 0;
      offset_ = 0;
      timer_.reset();
      energy_timer_.reset();
      if (sprite_.width()) { sprite_.fillSprite(0); }
    }

  private:
    int nghosts_ = 6;
    int grid_w_ = 320;
    int grid_h_ = 241;
    int grid_dx_ = 20;
    int grid_dy_ = 20;
    int d_offset_ = 5;
    int ghost_r_ = 50;
    LGFX_Sprite sprite_;
    TaskTimer timer_;
    TaskTimer energy_timer_;
    int offset_ = 0;
    int stage_ = 0;
};