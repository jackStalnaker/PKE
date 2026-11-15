#pragma once
#include "ghost.h"
#include "bigpumpkin_bmp.h"

inline const Ghost bigpumpkin {
    bigpumpkin_bmp,
    BIGPUMPKIN_WIDTH,
    BIGPUMPKIN_HEIGHT,
    0xFF8000, 
    5,
    "Pumpkin Goblin",
    "Spooky squash.\nEat your vegetables\non Halloween\nbefore they eat you.",
    5,
    3,
    3,
    {{0xE2, 0x00, 0x47, 0x05, 0xB5, 0x80, 0x68, 0x21, 0x2F, 0xEE, 0x01, 0x11}},
    1
};