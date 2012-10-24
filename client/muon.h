#ifndef MUON_H
#define MUON_H

#include <stdio.h>
#include <stdlib.h>

#include <darnit/darnit.h>

#include "view.h"

#define SCROLL_OFFSET 32
#define SCROLL_SPEED 4

DARNIT_PLATFORM platform;

DARNIT_MAP *map;
int map_w, map_h;

#endif