#pragma once

#include <pebble.h>

// Width of the fixed left-hand strip that holds the second-city block ("BKK"
// stacked above its time). The main clock centers in the space to its right.
// emery: wider strip to fit the larger secondary font on the taller screen.
#ifdef PBL_PLATFORM_EMERY
#define SECOND_CITY_BLOCK_W 60
#else
#define SECOND_CITY_BLOCK_W 44
#endif

void second_city_layer_create(Layer* parent_layer, GRect frame);

void second_city_layer_tick();

void second_city_layer_refresh();

void second_city_layer_destroy();
