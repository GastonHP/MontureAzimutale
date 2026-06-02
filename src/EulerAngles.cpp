
#include "EulerAngles.hpp"

static float normaliser180(float angle)
{
    while (angle < -180)
        angle += 360;
    while (angle >= 180)
        angle -= 360;
    return angle;
}
EulerAngles::EulerAngles(float roll, float pitch, float yaw)
{
    this->roll = normaliser180(roll);
    this->pitch = normaliser180(pitch);
    this->yaw = normaliser180(yaw);
}

// Fonction de conversion Quaternion -> Euler (Rad vers Deg)
EulerAngles EulerAngles::getEulerFromQuaternion(float qi, float qj, float qk, float qr)
{
    EulerAngles angles;

    // 1. ROLL (Rotation autour de l'axe X)
    float sinr_cosp = 2 * (qr * qi + qi * qk);
    float cosr_cosp = 1 - 2 * (qi * qi + qj * qj);
    angles.roll = atan2(sinr_cosp, cosr_cosp);

    // 2. PITCH (Rotation autour de l'axe Y - Votre Altitude)
    float sinp = 2 * (qr * qj - qk * qi);
    if (abs(sinp) >= 1)
        angles.pitch = copysign(M_PI / 2, sinp); // Utilise 90° si hors plage
    else
        angles.pitch = asin(sinp);

    // 3. YAW (Rotation autour de l'axe Z - Votre Azimut)
    float siny_cosp = 2 * (qr * qk + qi * qj);
    float cosy_cosp = 1 - 2 * (qj * qj + qk * qk);
    angles.yaw = -atan2(siny_cosp, cosy_cosp);

    // Conversion Radian -> Degrés
    angles.roll *= 180.0 / M_PI;
    angles.pitch *= 180.0 / M_PI;
    angles.yaw *= 180.0 / M_PI;

    // // Conversion quaternion → angles
    // angles.yaw = /* 90.0 + */ atan2(2.0f * (qr * qk + qi * qj), 1 - 2 * (qj * qj + qk * qk)) * 57.2958;
    // angles.pitch = asin(2.0f * (qr * qj - qk * qi)) * 57.2958;
    // angles.roll = atan2(2.0f * (qr * qi + qj * qk), 1 - 2 * (qi * qi + qj * qj)) * 57.2958;

    angles.roll = normaliser180(angles.roll);
    angles.pitch = normaliser180(angles.pitch);
    angles.yaw = normaliser180(angles.yaw);

    return angles;
}

EulerAngles convertQuaternionToEuler(float r, float i, float j, float k)
{
    EulerAngles angles;

    // 1. ROLL (Axe X)
    float sinr_cosp = 2 * (r * i + j * k);
    float cosr_cosp = 1 - 2 * (i * i + j * j);
    angles.roll = normaliser180(atan2(sinr_cosp, cosr_cosp) * RAD_TO_DEG);

    // 2. PITCH (Axe Y - Votre Altitude)
    float sinp = 2 * (r * j - k * i);
    if (abs(sinp) >= 1)
        angles.pitch = copysign(90.0, sinp); // Sécurité anti-NaN
    else
        angles.pitch = asin(sinp) * RAD_TO_DEG;

    // 3. YAW (Axe Z - Votre Azimut)
    float siny_cosp = 2 * (r * k + i * j);
    float cosy_cosp = 1 - 2 * (j * j + k * k);
    angles.yaw = normaliser180(atan2(siny_cosp, cosy_cosp) * RAD_TO_DEG);

    return angles;
}