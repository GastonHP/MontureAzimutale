
#pragma once
#include "Arduino.h"
#include "EulerAngles.hpp"

class MonEcran
{
public:
  static void setup();
  static void loop();
  static void logError(String s);
  static void log(String s);
  static void setActivated(bool active) { activated = active; }
  static void printf(const char *format, ...);

private:
  static void afficherBoussole(EulerAngles angles);
  static void afficherNiveauBulle(EulerAngles angles);
  static void afficherAngles(EulerAngles angles);
  static void afficherBatterie();
  static void afficherGPS();
  static void afficherQuadrilage(int step);
  static bool activated;
};