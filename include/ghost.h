#pragma once
#include "ghosts/gblogo_mono_bmp.h"

constexpr size_t EPC_LENGTH = 12;
constexpr size_t MAX_TAGS_PER_GHOST = 5;

/*
 * A ghost index entry.
 * @param bmp The bitmap of the ghost
 * @param bmp_width The width of the bitmap
 * @param bmp_height The height of the bitmap
 * @param color The color of the ghost
 * @param ghost_class The class of ghost (see https://ghostbusters.fandom.com/wiki/CDI_System)
 * @param name The name of this ghost
 * @param desc Description of this ghost
 * @param energy The energy level of the ghost
 * @param charge The charge level of the ghost
 * @param stantz Stanz radiation
 */
struct Ghost {
  const unsigned char* bmp = gblogo_mono_bmp;
  int bmp_width     = 256;
  int bmp_height    = 256;
  uint32_t color    = 0xFFFFFF; 
  int ghost_class   = 0;
  const char* name  = "Unknown Spirit";
  const char* desc  = "DANGER!";
  int energy        = 0;
  int charge        = 0;
  int stantz        = 9;

  uint8_t tag_epcs[MAX_TAGS_PER_GHOST][EPC_LENGTH] = {};
  uint8_t epc_count = 0;
};