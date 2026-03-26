#pragma once

#include <math.h>

// Structure pour stocker les angles Euler
class EulerAngles
{
public:
    EulerAngles(float yaw, float pitch, float roll);
    EulerAngles() : yaw(0), pitch(0), roll(0) {}
    
    // Fonction de conversion Quaternion -> Euler (Rad vers Deg)
    static EulerAngles getEulerFromQuaternion(float i, float j, float k, float r);
    float yaw;    // Azimut (Z)
    float pitch;  // Altitude (Y)
    float roll;   // Inclinaison latérale (X)
};