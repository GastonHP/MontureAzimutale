
#include "EulerAngles.hpp"

EulerAngles::EulerAngles(float roll, float pitch, float yaw)
{
    this->roll = roll;
    this->pitch = pitch;
    this->yaw = yaw;
}

// Fonction de conversion Quaternion -> Euler (Rad vers Deg)
EulerAngles EulerAngles::getEulerFromQuaternion(float i, float j, float k, float r)
{
    EulerAngles angles;

    // 1. ROLL (Rotation autour de l'axe X)
    float sinr_cosp = 2 * (r * i + j * k);
    float cosr_cosp = 1 - 2 * (i * i + j * j);
    angles.roll = atan2(sinr_cosp, cosr_cosp);

    // 2. PITCH (Rotation autour de l'axe Y - Votre Altitude)
    float sinp = 2 * (r * j - k * i);
    if (abs(sinp) >= 1)
        angles.pitch = copysign(M_PI / 2, sinp); // Utilise 90° si hors plage
    else
        angles.pitch = asin(sinp);

    // 3. YAW (Rotation autour de l'axe Z - Votre Azimut)
    float siny_cosp = 2 * (r * k + i * j);
    float cosy_cosp = 1 - 2 * (j * j + k * k);
    angles.yaw = atan2(siny_cosp, cosy_cosp);

    // Conversion Radian -> Degrés
    angles.roll *= 180.0 / M_PI;
    angles.pitch *= 180.0 / M_PI;
    angles.yaw *= 180.0 / M_PI;
    while (angles.roll < 0)
        angles.roll += 360; // Normaliser entre 0 et 360
    while (angles.roll >= 360)
        angles.roll -= 360;
    while (angles.pitch < 0)
        angles.pitch += 360; // Normaliser entre 0 et 360
    while (angles.pitch >= 360)
        angles.pitch -= 360;
    while (angles.yaw < 0)
        angles.yaw += 360; // Normaliser entre 0 et 360
    while (angles.yaw >= 360)
        angles.yaw -= 360;

    return angles;
}