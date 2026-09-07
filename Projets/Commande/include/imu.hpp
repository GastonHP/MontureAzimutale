#pragma once
#include "Arduino.h"
#include <Adafruit_BNO08x.h>
#include "../../Commun/include/Communication.h"
#include "EulerAngles.hpp"
#include "Capteur.hpp"

#define MAX_IMU_DATA 20

class Imu
{
public:
    // Public methods
    static void setup();
    static void loop();
    static bool saveIMUData(CapteursMessage *m);
    static CapteursMessage *hasNewData();

    static Capteur rotationCapteur;
    static Capteur ARVRCapteur;
    static Capteur gameRotationCapteur;
    static String IMUDataToString(CapteursMessage *data);

private:
    // Private members
    static CapteursMessage lastdata;
};