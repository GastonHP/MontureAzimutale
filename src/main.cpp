#include "commun.hpp"
#include "esp_infos.hpp"
#include "Config.hpp"
#include "GPSManager.hpp"

GPSManager gpsManager(18, 17, 9600); // RX=18, TX=17

extern void lcd_test_setup(Adafruit_ST7789 *monEcran);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  delay(2000); // Laisse le temps au S3 de se stabiliser
  // printInfo();
#ifndef TESTGPS
  monEcran_setup();
  delay(500);
#ifdef CONFIG_ACTIF
  Config::setup(monEcran_display);
  Log::addLog("Config::setup done.");
  generic_setup(monEcran_display);
  Log::addLog("generic_setup done.");
#endif

#ifdef SD_ACTIF
  SDisk::setup();
#endif
  bno08x_setup(&monEcran);
  Log::addLog("bno08x_setup done.");

  gpsManager.setup(&monEcran);
  Log::addLog("GPS_setup done.");

#else
  gpsManager.setup(nullptr);
#endif
}

unsigned long nextTime = 0;

void loop()
{
#ifndef TESTGPS
// if (millis() > nextTime)
// {
//   nextTime = millis() + 10000;
//   printInfo();
// }
#ifdef CONFIG_ACTIF
  generic_loop();
#endif
  bno08x_loop(&monEcran);
#endif
  gpsManager.loop();
}