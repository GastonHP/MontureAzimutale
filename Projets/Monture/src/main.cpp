#include "commun.hpp"
#include "esp_infos.hpp"
#include "Config.hpp"
#include "GPSManager.hpp"
#include "Batterie.hpp"
#include "OTA.hpp"
#include "log.hpp"

#include "EulerAngles.hpp"
#include "Telescope.hpp"
#include "MonServeur.hpp"
#include "monEcran.hpp"
#include "imu.hpp"

#ifdef TEST_LCD
extern void lcd_test_setup(Adafruit_ST7789 *monEcran);
#endif

static void logGen(String s, bool LogFile = true)
{
  Serial.println("==>" + s);
  MonEcran::log(s, 0);
  if (LogFile == true)
    Log::addLog(s);
}

void setup()
{
  Serial.begin(SERIAL_SPEED);
  // Laisse le temps au S3 de se stabiliser
  delay(2000);
  // setup de l'écran
  MonEcran::setup();
  logGen("MonEcran::setup done.", false);
  // setup de la LED RGB
  RGB_LED_setup();
  logGen("RGB_LED_setup done.", false);
  RGB_LED_SET(0, 0, 32, 15); // éteint la LED au démarrage
  // setup de la config wifi, mqtt, ftp, OTA, etc.
  Config::setup(MonEcran::log);
  logGen("Config::setup done.");
  RGB_LED_SET(0, 32, 32, 15); // éteint la LED au démarrage
  generic_setup(MonEcran::log);
  logGen("generic_setup done.");
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
  // setup du télescope (BNO08x, moteurs, écran)
  Telescope::setup();
  logGen("Telescope setup done.");
  // setup du serveur Web pour afficher les données et permettre le contrôle à distance
  MonServeur::setup();
  logGen("MonServeur setup done.");
  // setup du GPS
  GPSManager::setup(); // RX=18, TX=17
  logGen("GPS manager set.");
  Imu::setup();
  logGen("imu setup done!");
}

unsigned long nextTime = 0;
int count = 10;

void loop()
{
  if (!Telescope::loopActif)
    return;
  generic_loop();
  if (OTA::started())
  {
    Telescope::stop();              // Arrête les moteurs avant de faire quoi que ce soit d'autre
    MonServeur::setActivated(false); // Désactive le serveur Web pour éviter les conflits pendant l'OTA
    GPSManager::stop();
    return;
  }

  MonServeur::loop();
  GPSManager::loop();
  Telescope::loop();
  MonEcran::loop();
  if (millis() > nextTime)
  {
    nextTime = millis() + 5000;
    Log::addLog("Still alive!");
  }
}