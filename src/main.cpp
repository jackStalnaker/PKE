#include <string>
#include <map>
#include <math.h>
#include <Arduino.h>
#include "display.h"
#include "tasktimer.h"
#include "assets/gblogo_color.h"
#include "assets/gbtext.h"
#include "ghost.h"
#include "ghosts/ghosts.h"

LGFX display;
const uint16_t PHOSPHOR_GREEN = display.color565(80, 255, 100);
const uint16_t PHOSPHOR_CYAN  = display.color565(0, 255, 255);
const uint16_t DIM_GREEN      = display.color565(15, 70, 25);
const uint16_t PHOSPHOR_AMBER = display.color565(255, 180, 40);
const uint16_t ALERT_RED      = display.color565(255, 40, 30);
const uint16_t SAFETY_YELLOW  = display.color565(238, 210, 2);
const char* VERSION = {"2.0"};

const bool debug = false;

class Typewriter {
  public:
    Typewriter(lgfx::LovyanGFX& display, unsigned long delay_ms = 30)
      : display_(display), delay_ms_(delay_ms) {}

    bool type_text(const char* text) {
      if (index_ == 0) {
        l_margin_ = display_.getCursorX();
        text_len_ = strlen(text);
      }

      if (index_ == text_len_) {
        index_ = 0; 
        num_converted_ = false;
        return true;
      }

      if (index_ == 0 || timer_.expired(delay_ms_)) {
        if (text[index_] == '\n') {
          display_.setCursor(l_margin_, display_.getCursorY() + display_.fontHeight());
        } else {
          display_.print(text[index_]);
        }
        ++index_;
        timer_.reset();
      }
      return false;
    }

    bool type_text(int value) {
      if (!num_converted_) {
        snprintf(num_buffer_, sizeof(num_buffer_), "%d", value);
        num_converted_ = true;
      }
        return type_text(num_buffer_);
    }

  private:
    lgfx::LovyanGFX& display_;
    unsigned long delay_ms_;
    uint32_t l_margin_ = 0;
    size_t index_ = 0; 
    size_t text_len_ = 0;
    TaskTimer timer_;
    bool num_converted_ = false;
    char num_buffer_[12];
};

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

class SplashPage : public BasePage {
  public:
    SplashPage(lgfx::LovyanGFX& display)
        : BasePage(display) {}

    void draw() {
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
      display.fillRect(0, 440, 320, 40, TFT_BLACK);
      display_.drawString("If found call (713) 555-2368", 10, 460);
      delay(1500);
    }
};

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

class DetectionPage : public BasePage {
  public:
    DetectionPage(lgfx::LovyanGFX& display)
      : BasePage(display), typewriter_(display), warn_sprite_(&display)
    {
      make_detection_sprite();
    }

    void draw_UI(uint16_t bg_color = TFT_BLACK, uint16_t fg_color = PHOSPHOR_CYAN, uint16_t text_color = TFT_WHITE) {
      BasePage::draw_UI(bg_color, fg_color);
      display_.setFont(&fonts::AsciiFont8x16);
      display_.setTextColor(text_color);
      display_.setTextSize(1.5);
      display_.drawString(">> PKE 2.0 // DETECTION", 10, 10);
    }

    void make_detection_sprite() {
      warn_sprite_.setColorDepth(1);
      warn_sprite_.createSprite(310, 30);
      warn_sprite_.setBitmapColor(TFT_RED, TFT_BLACK);
      warn_sprite_.drawRect(0, 0, 310, 30, 1);
      warn_sprite_.setCursor(3, 0);
      warn_sprite_.setFont(&fonts::AsciiFont8x16);
      warn_sprite_.setTextColor(1);
      warn_sprite_.setTextSize(2);
      warn_sprite_.println("> ENTITY DETECTED <");
    }

    void draw(const Ghost* ghost) {
      switch (stage_) {
        case 0:
          draw_UI();
          snprintf(spk_buffer_, sizeof(spk_buffer_), "SPK%d-%04ld", ghost->ghost_class, random(9999));
          snprintf(energy_buffer_, sizeof(energy_buffer_), "Spectral energy 0x%04lX", random(0x10000));
          display_.setFont(&fonts::AsciiFont8x16);
          ghost_timer_.reset();
          ++stage_;
        
        case 1:
          if (ghost_timer_.expired(300)) {
            draw_ghost(ghost, display.width()/2, display.height()/2);
            draw_warning_triangle(240, 70, 60, PHOSPHOR_AMBER);
            ++stage_;
          }
          break;
        
        case 2:
          display_.setCursor(10, 65);
          display_.setTextSize(1);
          display_.setTextColor(PHOSPHOR_AMBER);
          ++stage_;

        case 3:
          if (typewriter_.type_text(spk_buffer_)) { ++stage_; }
          break;
        
        case 4:
          display_.setCursor(10, 80);
          display_.setTextSize(1);
          display_.setTextColor(PHOSPHOR_AMBER);
          ++stage_;

        case 5:
          if (typewriter_.type_text(energy_buffer_)) {++stage_; }
          break;

        case 6:
          display_.setCursor(10, 95);
          display_.setTextSize(1);
          display_.setTextColor(PHOSPHOR_AMBER);
          ++stage_;
        
        case 7:
          if (typewriter_.type_text("Spangler filter active")) { ++stage_; }
          break;

        case 8: 
          display_.setCursor(10, 110);
          ++stage_;

        case 9:
          if (typewriter_.type_text("Calibrating...")) {
            venkman_timer_.reset();
            ++stage_;
          }
          break; 

        case 10:
          if (venkman_timer_.expired(300)) {
            display_.setCursor(180, 350);
            display_.setTextColor(TFT_WHITE);
            display_.println("Q7 Venkman waves");
            display_.drawLine(180, 350, 160, 240, TFT_WHITE);
            ++stage_;
          }
          break;
      }

      if (warn_timer_.expired(300)) {
        display_.fillRect(5, 400, 310, 30, TFT_BLACK);
        if (warn_visible_) { warn_sprite_.pushSprite(5, 400); }
        warn_visible_ = !warn_visible_;
        warn_timer_.reset();
      }
    }

    void reset() {
      stage_ = 0;
      warn_visible_ = true;
      warn_timer_.reset();
    }

  private:
    int stage_ = 0;
    char spk_buffer_[25] = {};
    char energy_buffer_[25] = {};
    Typewriter typewriter_;
    TaskTimer warn_timer_;
    TaskTimer ghost_timer_;
    TaskTimer venkman_timer_;
    bool warn_visible_ = true;
    LGFX_Sprite warn_sprite_;
};

class AnalysisPage : public BasePage {
  public:
    AnalysisPage(lgfx::LovyanGFX& display)
      : BasePage(display), typewriter_(display) {}

    void draw_UI(uint16_t bg_color = TFT_BLACK, uint16_t fg_color = PHOSPHOR_CYAN, uint16_t text_color = TFT_WHITE) {
      BasePage::draw_UI(bg_color, fg_color);
      display_.setFont(&fonts::AsciiFont8x16);
      display_.setTextColor(text_color);
      display_.setTextSize(1.5);
      display_.drawString(">> PKE 2.0 // ANALYSIS", 10, 10);
      display_.drawFastHLine(0, 445, 320, fg_color);
    }

    void draw(const Ghost* ghost) {
      int gleft = 10;
      int gtop = 60;
      int gwidth = ghost->bmp_width / 2;
      int gheight = ghost->bmp_height / 2;
      int gbottom = gtop + gheight;

      switch (stage_) {
        case 0:
          draw_UI();
          display_.setFont(&fonts::AsciiFont8x16);

          draw_ghost_corner(ghost, 10, gtop, 0, 0.5, 0.5);
          display_.drawRect(gleft, gtop, gwidth, gheight, PHOSPHOR_CYAN);
          display_.drawLine(gwidth + gleft, gtop + 10, gwidth + 20, gtop + 5, PHOSPHOR_CYAN);
          display_.drawLine(gwidth + gleft + 10, gtop + 5, gwidth + 30, gtop + 5, PHOSPHOR_CYAN);
          display_.setCursor(gwidth + 35, gtop);
          display_.setTextColor(PHOSPHOR_CYAN);
          display_.setTextSize(1);
          display_.printf("SPECTRAL ENERGY");
          display_.setCursor(gwidth + 160, gtop + 22);
          display_.printf("kJ");

          display_.drawLine(gwidth + 10, gtop + gheight / 2, gwidth + 30, gtop + gheight / 2, PHOSPHOR_CYAN);
          display_.setCursor(gwidth + 35, gtop + gheight / 2 - 5);
          display_.printf("SPECTRAL CHARGE");
          display_.setCursor(gwidth + 160, gtop + 22 + gheight / 2 - 5);
          display_.printf("kC");

          display_.drawLine(gwidth + 10, gbottom - 10, gwidth + 20, gbottom - 5, PHOSPHOR_CYAN);
          display_.drawLine(gwidth + 20, gbottom - 5, gwidth + 30, gbottom - 5, PHOSPHOR_CYAN);
          display_.setCursor(gwidth + 30, gbottom - 12);
          display_.printf("STANTZ RAYS: ");
          display_.setTextColor(PHOSPHOR_GREEN);
          display_.printf("%+d", ghost->stantz);

          display_.setCursor(10, gbottom + 63);
          display_.setTextColor(PHOSPHOR_CYAN);
          display_.printf("      >> SPECTRAL SIGNATURE <<");
          ++stage_;
        
        case 1:
          display_.setCursor(10, gbottom + 100);
          display_.setTextColor(PHOSPHOR_AMBER);
          display_.setTextSize(1.5);
          ++stage_;
        
        case 2:
          if (typewriter_.type_text("Class: ")) { ++stage_; }
          break;
        
        case 3:
          display_.setTextColor(TFT_WHITE);
          display_.setTextSize(1.5);
          ++stage_;

        case 4:
          if (typewriter_.type_text(ghost->ghost_class)) { ++stage_; }
          break;

        case 5:
          display_.setCursor(10, gbottom + 130);
          display_.setTextColor(PHOSPHOR_AMBER);
          display_.setTextSize(1.5);
          ++stage_;

        case 6:
          if (typewriter_.type_text("Name: ")) { ++stage_; }
          break;

        case 7:
          display_.setTextColor(TFT_WHITE);
          display_.setTextSize(1.5);
          ++stage_;

        case 8:
          if (typewriter_.type_text(ghost->name)) { ++stage_; }
          break;

        case 9:
          display_.setCursor(10, gbottom + 160);
          display_.setTextColor(TFT_WHITE);
          display_.setTextSize(1.5);
          ++stage_;

        case 10:
          if (typewriter_.type_text(ghost->desc)) { ++stage_; }
          break;

        case 11:
          display_.setCursor(10, 455);
          display_.setTextSize(1);
          display_.setTextColor(TFT_RED);
          display_.printf("!!!!! CAUTION: RECOMMEND CAPTURE !!!!!");
          ++stage_;
      }

      if (graph_timer_.expired(50)) {
        float energy = random(ghost->energy + 1) / 10.0f;
        float charge = random(ghost->charge + 1) / 10.0f;
        draw_segment_bar(gwidth + 35, gtop + 20 + gheight / 2 - 5, 120, 15, 10, charge, PHOSPHOR_AMBER);
        draw_segment_bar(gwidth + 35, gtop + 20, 120, 15, 10, energy, PHOSPHOR_AMBER);
        draw_spectrum(10, gbottom + 20, 300, 40, PHOSPHOR_CYAN);
        graph_timer_.reset();
      }
    }
  
  private:
    Typewriter typewriter_;
    int stage_ = 0;
    TaskTimer graph_timer_;
};

DetectionPage detection_page(display);
BasePage base_page(display);
SplashPage splash_page(display);
AnalysisPage analysis_page(display);
ScannerPage scanner_page(display);

void setup() {
    Serial.begin(9600);
    display.init();
    display.setRotation(2);
    display.fillScreen(TFT_BLACK);
    // splash_page.draw();
}

void loop() {
  // scanner_page.draw();
  // detection_page.draw(ghosts["Big Pumpkin    "]);
  // analysis_page.draw(ghosts["Big Pumpkin    "]);
  // analysis_page.draw(ghosts["Left Pumpkin   "]);
  // analysis_page.draw(ghosts["Right Pumpkin  "]);
  // analysis_page.draw(ghosts["Evil Baby      "]);
  // analysis_page.draw(ghosts["PA Skull       "]);
  // analysis_page.draw(ghosts["Trumpet Skull  "]);
  // analysis_page.draw(ghosts["Knock Box      "]);
  // analysis_page.draw(ghosts["Generic Ghost 1"]);
  // analysis_page.draw(ghosts["Generic Ghost 2"]);
  // analysis_page.draw(ghosts["Generic Ghost 3"]);
  // analysis_page.draw(ghosts["Generic Ghost 4"]);
  analysis_page.draw(ghosts["Generic Ghost 5"]);
}