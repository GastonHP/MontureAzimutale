#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Adafruit_BNO08x.h>
#include "Log.hpp"
#include "FastLED.h"

// gestion écran
extern Adafruit_ST7789 monEcran;
extern void monEcran_setup();
extern void monEcran_display(String s);

// gestion GY-BNO08X
extern void bno08x_setup(Adafruit_ST7789 *monEcranptr); 
extern void bno08x_loop(Adafruit_ST7789 *monEcranptr);

// gestion RGB LED
extern void RGB_LED_setup();    
extern void RGB_LED_loop();
extern void RGB_LED_SET(fl::u8 ir, fl::u8 ig, fl::u8 ib, fl::u8 brightness);