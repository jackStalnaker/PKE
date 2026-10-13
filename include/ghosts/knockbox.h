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
    19
};