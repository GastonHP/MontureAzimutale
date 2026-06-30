#pragma once
#include "Arduino.h"
#include "../../Commun/include/Communication.h"
#include "EulerAngles.hpp"

#define MAX_IMU_DATA 20
const uint8_t NO_SENSOR_ID = -1;
const uint8_t STILL_ALIVE = -2;

class Imu
{
public:
    // Public methods
    static void setup();
    static void loop();
    static bool saveIMUData(IMUData *m);          // sauve le msg IMU dans la table
    static IMUData *getIMUData(uint8_t sensorId); // récupère le msg IMU
    static bool saveIMUDataBefore(uint8_t sensorId);
    static IMUData *getIMUDataBefore(uint8_t sensorId);
    static bool saveIMUDataAfter(uint8_t sensorId);
    static IMUData *getIMUDataAfter(uint8_t sensorId);
    static EulerAngles getEulerAngles(uint8_t sensorId);
    static EulerAngles getEulerAngles(IMUData *m);
    static IMUData *hasNewIMUData(uint8_t sensorId);
    static IMUData *hasNewIMUData();
    static void setTreated(uint8_t sensorId,bool treated);
    static void setTreated(bool treated);

private:
    // Private members
    static IMUData data[];
};