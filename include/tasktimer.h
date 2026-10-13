#pragma once
#include <Arduino.h>

/*
 * Simple pollable timer.
 */
class TaskTimer {
public:
  /*
   * Reset timer.
   */
  void reset() {
    previous_ = millis();
  }

  /*
   * @brief Is the timer expired?
   * @param interval Timer interval to test.
   */
  bool expired(unsigned long interval) {
    return millis() - previous_ >= interval;
  }

private:
  unsigned long previous_ = 0;
};