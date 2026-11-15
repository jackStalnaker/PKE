#pragma once
#include <Arduino.h>
#include "display.h"

/* 
 * Some custom color definitions
 */
const uint16_t PHOSPHOR_GREEN = lgfx::color565(80, 255, 100);
const uint16_t PHOSPHOR_CYAN  = lgfx::color565(0, 255, 255);
const uint16_t DIM_GREEN      = lgfx::color565(15, 70, 25);
const uint16_t PHOSPHOR_AMBER = lgfx::color565(255, 180, 40);
const uint16_t ALERT_RED      = lgfx::color565(255, 40, 30);
const uint16_t SAFETY_YELLOW  = lgfx::color565(238, 210, 2);