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
    3
};