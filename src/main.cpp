#include "commun.hpp"
#include "esp_infos.hpp"
#include "Config.hpp"
#include "GPSManager.hpp"
#include "Batterie.hpp"
#include "OTA.hpp"

#include "EulerAngles.hpp"
#include "Telescope.hpp"
#include "WebServer.hpp"

GPSManager gpsManager(18, 17, 9600); // RX=18, TX=17

extern void lcd_test_setup(Adafruit_ST7789 *monEcran);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  delay(2000); // Laisse le temps au S3 de se stabiliser
  RGB_LED_setup();
  Log::addLog("RGB_LED_setup done.");
  // setup de l'écran
  monEcran_setup();
  RGB_LED_SET(0, 0, 32, 15); // éteint la LED au démarrage
  delay(500);
  Serial.println(F("monEcran_setup done."));

  // setup de la config wifi, mqtt, ftp, OTA, etc.
  Config::setup(monEcran_display);
  Log::addLog("Config::setup done.");
  RGB_LED_SET(0, 32, 32, 15); // éteint la LED au démarrage
  Serial.println(F("Config::setup done."));
  generic_setup(monEcran_display);
  Log::addLog("generic_setup done.");

  Serial.println(F("Setup after config done."));
  monEcran_display("Setup done.");
#ifdef SD_ACTIF
  SDisk::setup();
  Serial.println(F("Setup after SDisk done."));
#endif
  unsigned long time = millis() + 0; // Attendre 10 secondes pour laisser le temps à l'utilisateur de voir les messages précédents
  while (millis() < time)
  {
    // Attente active pour ne pas bloquer les autres tâches (comme l'OTA)
    generic_loop();
  }
  // setup du GPS
  gpsManager.setup(&monEcran);
  monEcran_display("GPS setup done.");
  Serial.println(F("GPS_setup done."));
  Log::addLog("GPS_setup done.");
  Serial.println(F("Setup after GPS done."));
  // setup du télescope (BNO08x, moteurs, écran)
  Telescope::setup(&monEcran);
  monEcran_display("Telescope setup done.");
  Telescope::setGPSManager(&gpsManager);
  monEcran_display("Telescope GPS manager set.");
  Serial.println(F("Telescope::setup done."));
  Log::addLog("Telescope::setup done.");
  // Telescope::steps(-100, 0); // Exemple : déplacer les moteurs de 100 pas
  EulerAngles angles = Telescope::getCurrentAngles();
  angles.roll = 0.0; // Simuler une inclinaison pour tester le niveau à bulle
  // Telescope::setTarget(angles, 0.5, 50); // Cible les angles actuels pour tester le mode automatique
  WebServer::setup(&gpsManager);
}

unsigned long nextTime = 0;
int count = 10;

void loop()
{
  generic_loop();
  if (OTA::started())
  {
    Telescope::stop(); // Arrête les moteurs avant de faire quoi que ce soit d'autre
    WebServer::loop(OTA::started());
    return;
  }

  WebServer::loop(OTA::started());
  gpsManager.loop();
  Telescope::loop();
  Batterie::loop(&monEcran);
  // if (count > 0 && millis() > nextTime)
  // {
  //   nextTime = millis() + 2000;
  //   count--;
  //   Telescope::steps(-100, 100); // Exemple : déplacer les moteurs de 100 pas
  // }
  // if (count == 0 && millis() > nextTime)
  // {
  //   nextTime = millis() + 2000;
  //   count--;
  //   Telescope::steps(1000, -1000); // Exemple : déplacer les moteurs de 100 pas
  // }
}