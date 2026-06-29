#pragma once
#include "Arduino.h"
#include "../../Commun/include/Communication.h"
#include "EulerAngles.hpp"

#define MAX_IMU_DATA 5
class Imu
{
public:
    
    // Public methods
    static void setup();
    static void loop();
    static bool addMessage(struct_message *m);
    static struct_message *getMessage(int sensorId);
    static EulerAngles getEulerAngles(int sensorId);

    private:
    // Private members
    static struct_message  data[];
};