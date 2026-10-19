#pragma once
#include "Arduino.h"
#include <Adafruit_BNO08x.h>
#include "../../Commun/include/Communication.h"

// pour chaque capteur, on garde les 20 dernières valeurs reçues
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
    bool addImuData(MessageData *data);
    MessageData *getData(int index = 0);
    int nbData();

    static void setup();
    static void loop();
    static bool saveIMUData(MessageData *m);
    static MessageData *hasNewData();

    static Capteur rotationCapteur;
    static Capteur ARVRCapteur;
    static Capteur gameRotationCapteur;
    static String IMUDataToString(MessageData *data);
    static String getSummary();

private:
    Capteur::TYPE typeImu;
    MessageData data[MAX_IMU_DATA];
    static bool areEqual(MessageData *a, MessageData *b);

    static MessageData lastdata;
    static int nbDataReceived;
};