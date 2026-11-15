#pragma once
#include <Arduino.h>
#include "display.h"
#include "customcolors.h"
#include "basepage.h"
#include "tasktimer.h"
#include "typewriter.h"
#include "ghost.h"

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

    /*
     * Draw the page. 
     * Return true if still drawing, false if done.
     */
    bool draw(const Ghost* ghost, unsigned long hold=0) {
      int gleft = 10;
      int gtop = 60;
      int gwidth = ghost->bmp_width / 2;
      int gheight = ghost->bmp_height / 2;
      int gbottom = gtop + gheight;

      if (!is_done_) {   // separate out the parts that should keep running even when done and holding
        switch (stage_) {
          case 0:
            display_.fillScreen(TFT_BLACK);
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
            hold_timer_.reset();
            ++stage_;
            break;
          
          case 12:
            if (hold_timer_.expired(hold)) {
              ++stage_;
            }
            break;
        }
      }

      if (graph_timer_.expired(50)) {
        float energy = random(ghost->energy + 1) / 10.0f;
        float charge = random(ghost->charge + 1) / 10.0f;
        draw_segment_bar(gwidth + 35, gtop + 20 + gheight / 2 - 5, 120, 15, 10, charge, PHOSPHOR_AMBER);
        draw_segment_bar(gwidth + 35, gtop + 20, 120, 15, 10, energy, PHOSPHOR_AMBER);
        draw_spectrum(10, gbottom + 20, 300, 40, PHOSPHOR_CYAN);
        graph_timer_.reset();
      }
      is_done_ = stage_ == 13;
      return !is_done_;
    }

    void reset() {
      stage_ = 0;
      is_done_ = false;
      graph_timer_.reset();
      hold_timer_.reset();
    }

    bool is_done() {
      return is_done_;
    }
  
  private:
    Typewriter typewriter_;
    int stage_ = 0;
    bool is_done_ = false;
    TaskTimer graph_timer_;
    TaskTimer hold_timer_;
};