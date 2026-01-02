#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

#include "esp_infos.hpp"

// Define pins for the TFT display
#define TFT_CS 10   // Chip select pin
#define TFT_DC 4 //13   // Data Command pin
#define TFT_RST 14  // Reset pin (could connect to Arduino RESET pin)
#define TFT_MOSI 11 // Data out
#define TFT_SCLK 12 // Clock out

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

extern void lcd_test_setup(Adafruit_ST7789 *tft);
extern void bno08x_setup(Adafruit_ST7789 *tftptr);
extern void bno08x_loop(Adafruit_ST7789 *tftptr);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  delay(2000); // Laisse le temps au S3 de se stabiliser
  printInfo();

  SPI.begin(12, 13, 11); // SCK, MISO, MOSI

  tft.init(240, 320);
  tft.setRotation(0);
  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  // tft.setSPISpeed(40000000);
  Serial.println(F("Initialized"));
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);
  //lcd_test_setup(&tft);
  bno08x_setup(&tft);
}

unsigned long nextTime = 0;

void loop()
{
  if (millis() > nextTime)
  {
    nextTime = millis() + 10000;
    printInfo();
  }
  bno08x_loop(&tft);
}