#pragma once
#include "ghost.h"
#include "knockbox_bmp.h"

inline const Ghost knockbox {
    knockbox_bmp,
    KNOCKBOX_WIDTH,
    KNOCKBOX_HEIGHT,
    0xFF8000, 
    2,
    "The Warren Box",
    "Who's there?\n...\n....",
    10,
    10,
    19,
    {{0xE2, 0x00, 0x47, 0x06, 0x6D, 0x40, 0x68, 0x21, 0x3B, 0x6A, 0x01, 0x13}},
    1
};