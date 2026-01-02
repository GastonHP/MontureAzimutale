#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// Define pins for the TFT display
#define TFT_CS 10   // Chip select pin
#define TFT_DC 4    // 13   // Data Command pin
#define TFT_RST 14  // Reset pin (could connect to Arduino RESET pin)
#define TFT_MOSI 11 // Data out
#define TFT_SCLK 12 // Clock out

Adafruit_ST7789 monEcran = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void monEcran_setup()
{
    SPI.begin(12, 13, 11); // SCK, MISO, MOSI

    monEcran.init(240, 320);
    monEcran.setRotation(0);
    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    // monEcran.setSPISpeed(40000000);
    Serial.println(F("monEcran initialized"));
    uint16_t time = millis();
    Serial.print("remplissage en noir...");
    monEcran.fillScreen(ST77XX_BLACK);
    Serial.println(" en " + String(millis() - time, DEC) + "ms.");
}