#pragma once
#include "Arduino.h"
#include <Adafruit_BNO08x.h>
#include "../../Commun/include/Communication.h"

#define MAX_IMU_DATA 20

class Capteur
{
public:
    enum TYPE
    {
        NO_SENSOR_ID = 0xff,
        STILL_ALIVE = 0xfe,
        ROTATION = SH2_ROTATION_VECTOR,
        ARVR = SH2_ARVR_STABILIZED_RV,
        GAME_ROTATION = SH2_GAME_ROTATION_VECTOR
    };
    Capteur(Capteur::TYPE t);
    bool addImuData(CapteursMessage *data);

private:
    Capteur::TYPE typeImu;
    CapteursMessage data[MAX_IMU_DATA];
    static bool areEqual(CapteursMessage *a, CapteursMessage *b);
};