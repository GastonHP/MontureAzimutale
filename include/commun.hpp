#pragma once

#include "FastLED.h"

// gestion RGB LED
extern void RGB_LED_setup();
extern void RGB_LED_loop();
extern void RGB_LED_SET(fl::u8 ir, fl::u8 ig, fl::u8 ib, fl::u8 brightness);