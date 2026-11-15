#pragma once
#include <Arduino.h>
#include "display.h"
#include "tasktimer.h"

/*
 * Class supporting text typing letter by letter.
 */
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