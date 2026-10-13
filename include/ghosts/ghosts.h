#pragma once
#include <map>
#include <string>
#include "ghost.h"
#include "ghosts/bigpumpkin.h"
#include "ghosts/smallpumpkinleft.h"
#include "ghosts/smallpumpkinright.h"
#include "ghosts/evilbaby.h"
#include "ghosts/paskull.h"
#include "ghosts/trumpetskull.h"
#include "ghosts/knockbox.h"
#include "ghosts/generic_class1.h"
#include "ghosts/generic_class2.h"
#include "ghosts/generic_class3.h"
#include "ghosts/generic_class4.h"
#include "ghosts/generic_class5.h"

// Ghost codex ================
std::map<std::string, const Ghost*> ghosts{
  { "Big Pumpkin    ", &bigpumpkin },
  { "Left Pumpkin   ", &smallpumpkinleft },
  { "Right Pumpkin  ", &smallpumpkinright },
  { "Evil Baby      ", &evilbaby },
  { "PA Skull       ", &paskull },
  { "Trumpet Skull  ", &trumpetskull },
  { "Knock Box      ", &knockbox },
  { "Generic Ghost 1", &generic_class1 },
  { "Generic Ghost 2", &generic_class2 },
  { "Generic Ghost 3", &generic_class3 },
  { "Generic Ghost 4", &generic_class4 },
  { "Generic Ghost 5", &generic_class5 }
//   // { "Unknown        ", { logo_bmp,         "?", "Unknown Spirit       ", "DANGER!             ", "  +999 kJ   -999 kC " } }
};