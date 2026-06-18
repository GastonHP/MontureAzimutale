#include "Batterie.hpp"
#include <Arduino.h>
#include "Log.hpp"
#include "monEcran.hpp"

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
