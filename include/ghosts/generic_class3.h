#pragma once
#include "ghost.h"
#include "generic_class3_bmp.h"

inline const Ghost generic_class3 {
    generic_class3_bmp,
    GENERIC_CLASS3_WIDTH,
    GENERIC_CLASS3_HEIGHT,
    0xFF8000, 
    3,
    "Human form I",
    "No identity.\nMay not purchase alcohol\nand reads emo poetry.",
    4,
    4,
    3,
    {{0xE2, 0x00, 0x47, 0x06, 0x6D, 0x30, 0x68, 0x21, 0x3B, 0x69, 0x01, 0x0F}},
    1
};