#pragma once
#include "ghost.h"
#include "evilpram_bmp.h"

inline const Ghost evilbaby {
    evilpram_bmp,
    EVILPRAM_WIDTH,
    EVILPRAM_HEIGHT,
    0xFF8000, 
    5,
    "Possessed Doll",
    "Tormented toy.\nDown will come\ncradle, baby and all.",
    8,
    7,
    7,
    {{0xE2, 0x00, 0x47, 0x06, 0x6A, 0xE0, 0x68, 0x21, 0x3B, 0x44, 0x01, 0x15}},
    1
};