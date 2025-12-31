#include <Arduino.h>
#include "esp_infos.hpp"
#ifdef FASTLED
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 48
int i = 0;

CRGB leds[NUM_LEDS];

void setup()
{
    FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);
    Serial.begin(115200);
    printInfo();
}

void loop()
{
    if (i != 4)
    {
        for (i = 0; i < 16; i++)
        {
            leds[0] = CRGB(i, 128+i*2,64+ i*4);
            FastLED.show();
            delay(500);

            //             leds[0] = CRGB::DarkRed;// CRGB::Red;
            // FastLED.show();
            // Serial.println("RED LED is ON");
            // delay(500);
            // leds[0] = CRGB::DarkGreen;
            // FastLED.show();
            // Serial.println("GREEN LED is ON");
            // delay(500);
            // leds[0] = CRGB::DarkBlue;
            // FastLED.show();
            // Serial.println("BLUE LED is ON");
            // delay(500);
            // leds[0] = CRGB::Black;
            // FastLED.show();
            // Serial.println("LED's are OFF");
            // delay(1500);
        }
    }
}
#endif