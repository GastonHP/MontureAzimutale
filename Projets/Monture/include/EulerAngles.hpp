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
    String toString() const { return "Y: " + String(yaw, 4) + "°, P: " + String(pitch, 4) + "°, R: " + String(roll, 4) + "°"; }
    EulerAngles copie() const { return EulerAngles(yaw, pitch, roll); }
    EulerAngles operator-(const EulerAngles &other) const { return EulerAngles(yaw - other.yaw, pitch - other.pitch, roll - other.roll); }
    EulerAngles operator+(const EulerAngles &other) const { return EulerAngles(yaw + other.yaw, pitch + other.pitch, roll + other.roll); }
    EulerAngles operator*(float factor) const { return EulerAngles(yaw * factor, pitch * factor, roll * factor); }
    EulerAngles operator/(float factor) const { return EulerAngles(yaw / factor, pitch / factor, roll / factor); }
    float yaw;   // Azimut
    float pitch; // Inclinaison
    float roll;  // Roulis

    float accuracy; // Précision des données (si disponible)

    uint64_t bno_timestamp; // Timestamp pour la validation des données
    uint64_t esp_timestamp; // Timestamp pour la validation des données
    uint8_t sensorId;       // ID du capteur pour identifier la source des données
    uint8_t precision;      // Précision des données (si disponible)
    bool valid = false;

private:
};