#pragma once
#include <Arduino.h>
#include "display.h"
#include "customcolors.h"
#include "basepage.h"
#include "tasktimer.h"
#include "typewriter.h"
#include "ghost.h"

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

    /*
     * Draw the page. 
     * Return true if still drawing, false if done.
     */
    bool draw(const Ghost* ghost, unsigned long hold=0) {
      // shortcut back
      if (is_done_) return false;

      switch (stage_) {
        case 0:
          display_.fillScreen(TFT_BLACK);
          draw_UI();
          snprintf(spk_buffer_, sizeof(spk_buffer_), "SPK%d-%04ld", ghost->ghost_class, random(9999));
          snprintf(energy_buffer_, sizeof(energy_buffer_), "Spectral energy 0x%04lX", random(0x10000));
          display_.setFont(&fonts::AsciiFont8x16);
          ghost_timer_.reset();
          ++stage_;
        
        case 1:
          if (ghost_timer_.expired(300)) {
            draw_ghost(ghost, display_.width()/2, display_.height()/2);
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
            hold_timer_.reset();
            ++stage_;
          }
          break;
        
        case 11:
          if (hold_timer_.expired(hold)) {
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

      is_done_ = stage_ == 12;
      return !is_done_;
    }
  
    bool is_done() {
      return is_done_;
    }

    void reset() {
      stage_ = 0;
      is_done_ = false;
      warn_visible_ = true;
      warn_timer_.reset();
      ghost_timer_.reset();
      venkman_timer_.reset();
      hold_timer_.reset();
    }

  private:
    int stage_ = 0;
    bool is_done_ = false;
    char spk_buffer_[25] = {};
    char energy_buffer_[25] = {};
    Typewriter typewriter_;
    TaskTimer warn_timer_;
    TaskTimer ghost_timer_;
    TaskTimer venkman_timer_;
    TaskTimer hold_timer_;
    bool warn_visible_ = true;
    LGFX_Sprite warn_sprite_;
};