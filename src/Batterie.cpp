#include "Batterie.hpp"
#include <Arduino.h>
#include "Log.hpp"

#define PIN_BATTERIE 1              // GPIO 1
const float facteurDivision = 11.0; // (315+51)/51

float Batterie::lireTension()
{

  static float lastV = 0.0;
  // Lecture brute (0 à 4095)
  float raw = (float)analogRead(PIN_BATTERIE);

  // Conversion en tension sur la pin (en tenant compte de l'atténuation 11dB par défaut)
  float vPin = (raw / 4095.0) * 3.3;

  // Facteur du pont : (R1 + R2) / R2 = (315+51) / 51  = 7.1764
  float vBat = vPin * 7.1764;
  if (lastV == 0.0)
    lastV = vBat;
  else
    lastV = 0.9 * lastV + 0.1 * vBat;

  // Calibration fine (ajustez ce petit décalage après mesure au multimètre)
  return lastV + 0.2;
}

void Batterie::loop(Adafruit_ST7789 *tftptr)
{
  static unsigned long lastDisplay = 0;
  if (millis() - lastDisplay < 10000)
    return;
  lastDisplay = millis();
  // Rien à faire pour l'instant, mais on peut ajouter des fonctionnalités plus tard
  tftptr->setCursor(120, 48);
  tftptr->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tftptr->printf("BAT:%.2fV", lireTension());
  Log::addLog("Batterie voltage: " + String(lireTension(), 2) + "V");
}
