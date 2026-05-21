
#pragma once
#include "Arduino.h"
#include "gpsManager.hpp"
#include "EulerAngles.hpp"

class MonEcran
{
public:
  static void setup();
  static void loop(GPSManager *gpsManager);
  static void logError(String s);
  static void log(String s);

  static void printf(const char *format, ...);
  private:
  static void afficherBoussole(EulerAngles angles);
  static void afficherNiveauBulle(EulerAngles angles);
  static void afficherPrecision(uint8_t precision);
  static void afficherAngles(EulerAngles angles);
};