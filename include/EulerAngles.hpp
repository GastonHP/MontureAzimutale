#pragma once
#include "Arduino.h"
#include <math.h>

// Structure pour stocker les angles Euler
class EulerAngles
{
public:
    EulerAngles(float yaw, float pitch, float roll);
    EulerAngles() : yaw(0), pitch(0), roll(0) {}

    // Fonction de conversion Quaternion -> Euler (Rad vers Deg)
    static EulerAngles getEulerFromQuaternion(float i, float j, float k, float r);
    bool isNull() const { return yaw * yaw + pitch * pitch + roll * roll < 0.001; };
    String toString() const
    {
        return "Yaw: " + String(yaw, 2)  + "°, Roll: " + String(roll, 2) + "°, Pitch: " + String(pitch, 2)+ "°";
    }
    EulerAngles copie() const { return EulerAngles(yaw, pitch, roll); }
    float yaw;   // Azimut 
    float pitch; // Roulis 
    float roll;  // Inclinaison 
};