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
    7
};