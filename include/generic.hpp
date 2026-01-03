#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Adafruit_BNO08x.h>

// gestion écran
extern Adafruit_ST7789 monEcran;
extern void monEcran_setup();

// gestion GY-BNO08X
extern void bno08x_setup(Adafruit_ST7789 *monEcranptr); 
extern void bno08x_loop(Adafruit_ST7789 *monEcranptr);

// gestion GPS NEO-6M
extern void GPS_setup(Adafruit_ST7789 *monEcranptr);
extern void GPS_loop(Adafruit_ST7789 *monEcranptr);