#pragma once
#include "ghost.h"
#include "paskull_bmp.h"

inline const Ghost paskull {
    paskull_bmp,
    PASKULL_WIDTH,
    PASKULL_HEIGHT,
    0xFF8000, 
    4,
    "Bone Daddy",
    "Elvis unpersonator.\nSings in the key\nof R flat.",
    6,
    3,
    11,
    {{0xE2, 0x00, 0x47, 0x06, 0x6D, 0x10, 0x68, 0x21, 0x3B, 0x67, 0x01, 0x0E}},
    1
};