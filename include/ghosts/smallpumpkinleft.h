#pragma once
#include "ghost.h"
#include "smallpumpkinleft_bmp.h"

inline const Ghost smallpumpkinleft {
    smallpumpkinleft_bmp,
    SMALLPUMPKINLEFT_WIDTH,
    SMALLPUMPKINLEFT_HEIGHT,
    0xFF8000, 
    5,
    "Pumpkin sprite",
    "Funky fruit.\nAsk it real nice\nfor a spice latte.",
    8,
    4,
    17,
    {{0xE2, 0x00, 0x47, 0x07, 0x24, 0xD0, 0x68, 0x21, 0x46, 0xE3, 0x01, 0x08}},
    1
};