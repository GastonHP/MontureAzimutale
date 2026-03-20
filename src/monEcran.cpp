#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "Log.hpp"

// Define pins for the TFT display
#define TFT_CS 10   // Chip select pin
#define TFT_DC 4    // 13   // Data Command pin
#define TFT_RST 14  // Reset pin (could connect to Arduino RESET pin)
#define TFT_MOSI 11 // Data out
#define TFT_MISO 13 // Data in
#define TFT_SCLK 12 // Clock out
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_ROTATION 0

Adafruit_ST7789 monEcran = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void monEcran_setup()
{
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI); // SCK, MISO, MOSI

    monEcran.init(TFT_WIDTH, TFT_HEIGHT);
    monEcran.setRotation(TFT_ROTATION);
    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    // monEcran.setSPISpeed(40000000);
    Serial.println(F("monEcran initialized"));
    Log::addLog("monEcran initialized");
    uint16_t time = millis();
    Serial.print("remplissage en noir...");
    monEcran.fillScreen(ST77XX_BLACK);
    Log::addLog("monEcran fillScreen done in " + String(millis() - time, DEC) + "ms");
    Serial.println(" en " + String(millis() - time, DEC) + "ms.");
}
void monEcran_display(String s)
{
    monEcran.setTextSize(2);
    monEcran.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    monEcran.println(s);
}
