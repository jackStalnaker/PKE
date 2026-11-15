#pragma once
#include <Arduino.h>
#include <math.h>
#include "display.h"
#include "customcolors.h"
#include "ghost.h"

bool debug = false;

/*
 * Base display page class with widgets defined.
 */
class BasePage {
  public:
    BasePage(lgfx::LovyanGFX& display) : display_(display) {}

    void draw_UI(uint16_t bg_color = TFT_BLACK, uint16_t fg_color = PHOSPHOR_CYAN) {
      if (!UI_drawn_) {
        display_.fillScreen(bg_color);
        display_.drawFastVLine(0, 0, 480, fg_color);
        display_.drawFastVLine(319, 0, 480, fg_color);
        display_.drawFastHLine(0, 0, 320, fg_color);
        display_.drawFastHLine(0, 40, 320, fg_color);
        display_.drawFastHLine(0, 479, 320, fg_color);
        UI_drawn_ = true;
      }
    }

    void reset_UI() {
      UI_drawn_ = false;
    }

    void draw_warning_triangle(int x, int y, int size, uint16_t color = TFT_YELLOW) {
        display_.drawTriangle(x, y + size, x + size / 2, y, x + size, y + size, color);
        display_.fillRect(x + size / 2 - size / 10, y + size / 3, size / 5, size / 3, color);
        display_.fillRect(x + size / 2 - size / 10, y + size * 2 / 3 + size / 12, size / 5, size / 6, color);
    }
    
    void draw_segment_bar(
        int x, int y,
        int width, int height,
        int segments,
        float value,
        uint16_t color = TFT_GREEN)
    {
        value = constrain(value, 0.0f, 1.0f);

        constexpr int gap = 2;
        int segmentWidth = (width - gap * (segments - 1)) / segments;
        int filled = round(value * segments);

        for (int i = 0; i < segments; ++i) {
            int sx = x + i * (segmentWidth + gap);

            display_.fillRect(sx, y, segmentWidth, height, TFT_BLACK);
            if (i < filled)
                display_.fillRect(sx, y, segmentWidth, height, color);
            else
                display_.drawRect(sx, y, segmentWidth, height, color);
        }
    }

    void draw_spectrum(int x, int y, int w, int h, uint16_t color = TFT_CYAN) {
        display_.fillRect(x, y, w, h, TFT_BLACK);
        display_.drawRect(x, y, w, h, color);

        float peak = random(35, 66) / 100.0f;
        float width = 0.13f;

        int prev_y = y + h - 5;
        int grid_dx = w / 5;

        for (int i = 1; i < w - 1; i++) {
            // float xgrid = x * pow((x + w)/x, i/10);
            // display.drawLine(xgrid, y, xgrid, y + h);
            if (i%grid_dx == 0) {
              display_.drawLine(x + i, y, x + i, y+h);
            }
            float t = float(i) / (w - 1);

            // Bell-shaped peak
            float d = (t - peak) / width;
            float amplitude = expf(-0.5f * d * d);

            // Random jaggedness
            float noise = random(-100, 101) / 100.0f;

            float signal = 0.08f + 0.78f * amplitude
                          + 0.2 * noise;

            signal = constrain(signal, 0.0f, 1.0f);

            int py = y + h - 2 - int(signal * (h - 4));

            if (i > 1) {
                display_.drawLine(x + i - 1, prev_y,
                                x + i, py, color);
            }

            prev_y = py;
        }
    }

    LGFX_Sprite make_ghost_sprite(const Ghost* ghost) {
      LGFX_Sprite sprite(&display_);
      sprite.setColorDepth(1);
      sprite.createSprite(ghost->bmp_width, ghost->bmp_height);
      sprite.setBitmapColor(display_.color565(ghost->color >> 16, (ghost->color >> 8) & 0xFF, ghost->color & 0xFF), TFT_BLACK);
      sprite.drawBitmap(0, 0, ghost->bmp, ghost->bmp_width, ghost->bmp_height, 1);
      if (debug) {
        sprite.drawRect(0, 0, ghost->bmp_width, ghost->bmp_height, TFT_RED);
      }
      return sprite;
    }

    void draw_ghost(const Ghost* ghost, int x, int y, float angle = 0, float scale_x = 1.0, float scale_y = 1.0) {
      LGFX_Sprite sprite = make_ghost_sprite(ghost);
      sprite.pushRotateZoom(x, y, angle, scale_x, scale_y);
    }

    void draw_ghost_corner(const Ghost* ghost, int x, int y, float angle = 0, float scale_x = 1.0, float scale_y = 1.0) {
      x += scale_x * ghost->bmp_width / 2;
      y += scale_y * ghost->bmp_height / 2;
      draw_ghost(ghost, x, y, angle, scale_x, scale_y);
    }


    void draw_battery(int x, int y, int w, int h, int percent) {
      percent = constrain(percent, 0, 100);

      uint16_t fg = percent <= 20 ? TFT_RED : PHOSPHOR_CYAN;

      // Battery outline
      display_.drawRoundRect(x, y, w, h, 3, PHOSPHOR_CYAN);

      // Positive terminal
      display_.fillRoundRect(x + w + 2, y + h / 3, 3, h / 3, 1, PHOSPHOR_CYAN);

      // Interior
      display_.fillRoundRect(x + 3, y + 3, w - 6, h - 6, 1, TFT_BLACK);

      // Charge level
      int fill_w= (w - 6) * percent / 100;
      if (fill_w> 0)
          display_.fillRect(x + 3, y + 3, fill_w, h - 6, fg);
    }


  protected:
    lgfx::LovyanGFX& display_;

  private:
    bool UI_drawn_ = false;
};
