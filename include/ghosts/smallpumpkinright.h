#pragma once
#include "ghost.h"
#include "smallpumpkinright_bmp.h"

inline const Ghost smallpumpkinright {
    smallpumpkinright_bmp,
    SMALLPUMPKINRIGHT_WIDTH,
    SMALLPUMPKINRIGHT_HEIGHT,
    0xFF8000, 
    5,
    "Pumpkin Imp",
    "Ghastly gourd.\nThis pumpkin might\nbe chunkin' YOU.",
    9,
    2,
    5,
    {{0xE2, 0x00, 0x47, 0x05, 0xB7, 0x00, 0x68, 0x21, 0x30, 0x06, 0x01, 0x14}},
    1
};