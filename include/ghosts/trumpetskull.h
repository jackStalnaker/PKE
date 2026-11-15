#pragma once
#include "ghost.h"
#include "trumpetskull_bmp.h"

inline const Ghost trumpetskull {
    trumpetskull_bmp,
    TRUMPETSKULL_WIDTH,
    TRUMPETSKULL_HEIGHT,
    0xFF8000, 
    4,
    "Louis Armsgone",
    "I said BLEW, not BOOOOO!",
    2,
    5,
    0,
    {{0xE2, 0x00, 0x47, 0x06, 0x6D, 0x20, 0x68, 0x21, 0x3B, 0x68, 0x01, 0x10}},
    1
};