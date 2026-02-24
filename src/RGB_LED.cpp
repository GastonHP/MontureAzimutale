#include <Arduino.h>
#include "esp_infos.hpp"
#ifdef USE_RGB_LED
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 48
int i = 0;

CRGB leds[NUM_LEDS];

void RGB_LED_SET(fl::u8 ir, fl::u8 ig, fl::u8 ib, fl::u8 brightness)
{
    FastLED.setBrightness(brightness);
    leds[0] = CRGB(ir, ig, ib);
    FastLED.show();
}

void RGB_LED_setup()
{
    FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);
    RGB_LED_SET(0, 32, 0, 15); // éteint la LED au démarrage
}



#endif