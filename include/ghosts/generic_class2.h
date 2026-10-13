#pragma once
#include "ghost.h"
#include "generic_class2_bmp.h"

inline const Ghost generic_class2 {
    generic_class2_bmp,
    GENERIC_CLASS2_WIDTH,
    GENERIC_CLASS2_HEIGHT,
    0xFF8000, 
    2,
    "Focused form",
    "May possess objects.\nYou know, like a\nghost, not own them.",
    3,
    3,
    2
};