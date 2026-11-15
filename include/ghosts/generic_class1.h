#pragma once
#include "ghost.h"
#include "generic_class1_bmp.h"

inline const Ghost generic_class1 {
    generic_class1_bmp,
    GENERIC_CLASS1_WIDTH,
    GENERIC_CLASS1_HEIGHT,
    0xFF8000, 
    1,
    "Indistinct form",
    "Vague haunting.\nMaybe it was just\nthe wind?",
    2,
    2,
    1,
    {{0xE2, 0x00, 0x47, 0x07, 0x29, 0x10, 0x68, 0x21, 0x47, 0x27, 0x01, 0x0C}},
    1
};