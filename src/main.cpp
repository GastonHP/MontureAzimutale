#include "commun.hpp"
#include "esp_infos.hpp"
#include "Config.hpp"
#include "GPSManager.hpp"
#include "Batterie.hpp"
#include "OTA.hpp"

GPSManager gpsManager(18, 17, 9600); // RX=18, TX=17

extern void lcd_test_setup(Adafruit_ST7789 *monEcran);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  delay(2000); // Laisse le temps au S3 de se stabiliser
  RGB_LED_setup();
  Log::addLog("RGB_LED_setup done.");
#ifndef TESTGPS
  monEcran_setup();
  RGB_LED_SET(0, 0, 32, 15); // éteint la LED au démarrage
  delay(500);
  Serial.println(F("monEcran_setup done."));
#ifdef CONFIG_ACTIF
  Config::setup(monEcran_display);
  Log::addLog("Config::setup done.");
  RGB_LED_SET(0, 32, 32, 15); // éteint la LED au démarrage
  Serial.println(F("Config::setup done."));
  generic_setup(monEcran_display);
  Log::addLog("generic_setup done.");
#endif
Serial.println(F("Setup after config done."));
#ifdef SD_ACTIF
  SDisk::setup();
#endif
Serial.println(F("Setup after SDisk done."));
  bno08x_setup(&monEcran);
  Serial.println(F("bno08x_setup done."));
  Log::addLog("bno08x_setup done.");

  gpsManager.setup(&monEcran);
  Serial.println(F("GPS_setup done."));
  Log::addLog("GPS_setup done.");
Serial.println(F("Setup after GPS done."));
#else
  gpsManager.setup(nullptr);
#endif
}

unsigned long nextTime = 0;

void loop()
{
#ifdef CONFIG_ACTIF
  generic_loop();
#endif
  if (!OTA::started())
  {
    bno08x_loop(&monEcran);
    gpsManager.loop();
    Batterie::loop(&monEcran);
  }
}