#pragma once

#include "Arduino.h"
#include "astronomy.h"
#include <Adafruit_BNO08x.h>
#include "EulerAngles.hpp"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include "MotorControl.hpp"
#include "GPSManager.hpp"

class Telescope
{
public:
    static bool loopActif;
    static void setup(Adafruit_ST7789 *tftptr);
    static void loop();
    static void setTarget(EulerAngles target, float tolerance, float maxSpeed);
    static void setAutomatique(bool autoMode);
    static bool isAutomatique() { return automatique; }
    static void pointer(float ascensionDroite, float declinaison);
    static void setGPSManager(GPSManager *gps) { gpsManager = gps; }
    static void commanderMouvement(float cibleAz, float cibleAlt);
    static void setMicrostepping(int ms);
    static void calibrate();
    static void steps(long stepsAz, long stepsAlt);
    static bool isMoving() { return motorAZ.isMoving() || motorALT.isMoving(); }
    static EulerAngles getCurrentAngles(bool forceUpdate = false);
    static void stop();
    static void calibrateMovement();
    static void log(String s);

    // OK
    static EulerAngles getDeltaAZ() { return deltaAZ; }
    static EulerAngles getDeltaALT() { return deltaALT; }
    static float getAccuracy() { return accuracy; }
    static uint8_t getPrecision() { return precision; }
    static String getJson();

private:
    static void display_angles(EulerAngles angles);
    static void dessinerBoussole(EulerAngles angles, bool vraiNord = true);
    static void dessinerNiveauVif(EulerAngles angles);
    static void setReports();
    static float calculerAzimutVrai(float azimutMagnetique);
    static MotorControl motorAZ;
    static MotorControl motorALT;

    static EulerAngles anglesActuels;
    static EulerAngles anglesAAtteindre;
    static float tolerance;
    static float maxSpeed;

    static Adafruit_BNO08x bno08x; // Utilisation du même reset pin que dans bno08x.cpp
    static sh2_SensorValue_t sensorValue;
    static Adafruit_ST7789 *tftptr;

    static bool setupOK;
    static bool automatique;
    static GPSManager *gpsManager;

    // OK
    static EulerAngles deltaAZ;
    static EulerAngles deltaALT;

    static float accuracy;
    static uint8_t precision;

    static const int maxLogLines = 20;
    static String logBuffer[maxLogLines];
};