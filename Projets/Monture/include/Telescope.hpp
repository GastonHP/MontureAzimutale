#pragma once

#include "Arduino.h"
// #include "astronomy.h"
#include <Adafruit_BNO08x.h>
#include "EulerAngles.hpp"
#include "MotorControl.hpp"
#include "calibration.hpp"

class Telescope
{
public:
    static bool loopActif;
    static void setup();
    static void loop();
    static void setTarget(EulerAngles target, float tolerance, float maxSpeed);
    static void setAutomatique(bool autoMode);
    static bool isAutomatique() { return automatique; }
    static void pointer(float ascensionDroite, float declinaison);
    static void commanderMouvement(float cibleAz, float cibleAlt);
    static void setMicrostepping(int ms);
    static void calibrate();

    static bool isMoving() { return motorAZ.isMoving() || motorALT.isMoving(); }
    // static EulerAngles getCurrentAngles(bool forceUpdate = false);
    static void readAnglesFromSensor(bool forceUpdate = false);
    static void stop();

    static void log(String s);
    static float calculerAzimutVrai(float azimutMagnetique);
    static bool isBno085Initialized() { return bno085_initialized; }
    // static uint8_t getPrecision() { return precision; }
    // static bool addCommande(int cmd)
    // {
    //     if (nbCommandes < maxCommandes)
    //     {
    //         commandes[nbCommandes++] = cmd;
    //         return true;
    //     }
    //     return false;
    // }
    static bool addCommande(int cmd, long parm1 = 0, long parm2 = 0)
    {
        log("Ajout de la commande " + String(cmd) + " avec parm1=" + String(parm1) + " et parm2=" + String(parm2));
        if (nbCommandes < maxCommandes)
        {
            commandes[nbCommandes] = cmd;
            parms1[nbCommandes] = parm1;
            parms2[nbCommandes] = parm2;
            nbCommandes++;
            return true;
        }
        log("Impossible d'ajouter la commande " + String(cmd) + " : nombre maximum de commandes atteint (" + String(maxCommandes) + ")");
        return false;
    }

    // OK
    static String getJson();

    static EulerAngles ARVR_STABILIZED_RV_anglesActuels;
    static EulerAngles ROTATION_VECTOR_anglesActuels;
    static EulerAngles GAME_ROTATION_VECTOR_anglesActuels;

    static Calibration ARVR_STABILIZED_RV_calibration;
    static Calibration ROTATION_VECTOR_calibration;
    static Calibration GAME_ROTATION_VECTOR_calibration;

    static const int CMD_CalibrateMouvement = 1;
    static const int CMD_Steps = 2;
    static const int CMD_CalibrateAZ = 3;
    static const int CMD_CalibrateALT = 4;

private:
    static void display_angles(EulerAngles angles);
    static void dessinerBoussole(EulerAngles angles, bool vraiNord = true);
    static void dessinerNiveauVif(EulerAngles angles);
    static void setReports();
    static void calibrateMovement();
    static void calibrateAZ();
    static void calibrateALT();
    static void steps(long stepsAz, long stepsAlt);

    static int nbCommandes;
    static const int maxCommandes = 10;
    static int commandes[]; // Stockage des commandes pour le calibrage du mouvement
    static long parms1[];   // Stockage des paramètres pour les commandes de calibrage du mouvement
    static long parms2[];   // Stockage des paramètres pour les commandes de calibrage du mouvement

    static MotorControl motorAZ;
    static MotorControl motorALT;

    static EulerAngles anglesAAtteindre;
    static float tolerance;
    static float maxSpeed;

    static Adafruit_BNO08x bno08x; // Utilisation du même reset pin que dans bno08x.cpp
    static sh2_SensorValue_t sensorValue;
    static bool bno085_initialized;

    static bool setupOK;
    static bool automatique;

    static const int maxLogLines = 20;
    static String logBuffer[maxLogLines];
};