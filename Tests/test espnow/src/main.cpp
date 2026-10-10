
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "config.hpp"
#ifdef USE_TEMP_SENSOR
#include <DallasTemperature.h>
// --- DS18B20 ---
#define ONE_WIRE_BUS 32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#endif
// --- Écran ST7789 (côté gauche ESP32) ---
#define TFT_CS 27
#define TFT_DC 26
#define TFT_RST 25
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_MISO 12 // non utilisé mais requis par SPI.begin()

// --- Backlight ---
#define TFT_BL 33
#define BL_CHANNEL 0

// --- Écran ---
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  Serial.println(String(millis(), DEC) + "Démarrage du test espnow...");

  // --- PWM Backlight ---
  Serial.println(String(millis(), DEC) + " :Initialisation du PWM pour le backlight...");
  ledcSetup(BL_CHANNEL, 5000, 8);
  ledcAttachPin(TFT_BL, BL_CHANNEL);
  ledcWrite(BL_CHANNEL, 63);

  // --- SPI écran ---
  Serial.println(String(millis(), DEC) + " :Initialisation du SPI pour l'écran...");
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  // --- Init écran ---
  Serial.println(String(millis(), DEC) + " :Initialisation de l'écran...");
  tft.init(240, 320);
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Test ESP-NOW"); 
    // setup de la config wifi, mqtt, ftp, OTA, etc.
  Config::setup();
  tft.println("Config::setup done.");
}

void gen_loop()
{

}

void loop()
{
  gen_loop();
}
