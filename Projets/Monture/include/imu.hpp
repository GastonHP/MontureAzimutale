#pragma once
#include "Arduino.h"
#include "../../Commun/include/Communication.h"
#include "EulerAngles.hpp"

#define MAX_IMU_DATA 10
const uint8_t NO_SENSOR_ID = -1;
const uint8_t STILL_ALIVE = -2;

class Imu
{
public:
    // Public methods
    static void setup();
    static void loop();
    static bool addMessage(struct_message *m);
    static struct_message *getMessage(uint8_t sensorId);
    static bool saveMessage(uint8_t sensorId);
    static struct_message *getSavedMessage(uint8_t sensorId) ;
    static struct_message *getNewMessage();
    static EulerAngles getEulerAngles(uint8_t sensorId);

private:
    // Private members
    static struct_message data[];
};