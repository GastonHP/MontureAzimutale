// // Tes pins
// // Azimut : STEP=5, DIR=6
// // Altitude : STEP=7, DIR=15
// // EN commun = 16

// Mount mount(5, 6, 7, 15, 16,
//             8.88f, 8.88f,   // steps/deg (exemple)
//             50.74f);        // latitude Mouscron

// void setup() {
//     mount.begin();
// }

// void loop() {
//     // Exemple : Sirius
//     float RA  = 6.7525f;
//     float DEC = -16.70f;

//     float LST = 12.345f;   // à calculer via GPS
//     float imuAz  = yaw;    // BNO085
//     float imuAlt = pitch;  // BNO085

//     // Offsets issus de AlignTwoStars
//     // mount.setOffsets(offsetAz, offsetAlt);

//     mount.gotoRaDec(RA, DEC, LST, imuAz, imuAlt);

//     while (true);
// }



#pragma once
#include <math.h>

// ---------- Utils angles ----------
static float m_degToRad(float d) { return d * M_PI / 180.0; }
static float m_radToDeg(float r) { return r * 180.0 / M_PI; }

static float m_normalize360(float a) {
    while (a < 0) a += 360.0;
    while (a >= 360) a -= 360.0;
    return a;
}

static float m_normalize180(float a) {
    while (a < -180) a += 360.0;
    while (a > 180)  a -= 360.0;
    return a;
}

// ---------- RA/DEC -> Az/Alt ----------
static void m_raDecToAzAlt(float RA_hours, float DEC_deg,
                           float lat_deg, float LST_hours,
                           float &Az_deg, float &Alt_deg)
{
    float lat = m_degToRad(lat_deg);
    float dec = m_degToRad(DEC_deg);

    float HA_hours = LST_hours - RA_hours;
    float HA = m_degToRad(HA_hours * 15.0);

    float sinAlt = sin(dec)*sin(lat) + cos(dec)*cos(lat)*cos(HA);
    float Alt = asin(sinAlt);

    float cosAz = (sin(dec) - sin(Alt)*sin(lat)) / (cos(Alt)*cos(lat));
    float Az = acos(cosAz);

    if (sin(HA) > 0)
        Az = 2*M_PI - Az;

    Az_deg  = m_normalize360(m_radToDeg(Az));
    Alt_deg = m_radToDeg(Alt);
}

// ---------- Classe Mount ----------
class Mount {
public:
    struct Offsets {
        float offsetAz  = 0.0f;
        float offsetAlt = 0.0f;
    };

private:
    // Pins moteurs
    int stepAzPin, dirAzPin;
    int stepAltPin, dirAltPin;
    int enPin;

    // Steps / degré
    float stepsPerDegAz;
    float stepsPerDegAlt;

    // Offsets d’alignement
    Offsets off;

    // Latitude
    float latitude_deg;

public:
    Mount(int stepAz, int dirAz,
          int stepAlt, int dirAlt,
          int en,
          float stepsDegAz, float stepsDegAlt,
          float lat_deg)
    : stepAzPin(stepAz), dirAzPin(dirAz),
      stepAltPin(stepAlt), dirAltPin(dirAlt),
      enPin(en),
      stepsPerDegAz(stepsDegAz),
      stepsPerDegAlt(stepsDegAlt),
      latitude_deg(lat_deg)
    {}

    void begin() {
        pinMode(stepAzPin, OUTPUT);
        pinMode(dirAzPin, OUTPUT);
        pinMode(stepAltPin, OUTPUT);
        pinMode(dirAltPin, OUTPUT);
        pinMode(enPin, OUTPUT);
        digitalWrite(enPin, LOW); // enable
    }

    void setOffsets(float offsetAz_deg, float offsetAlt_deg) {
        off.offsetAz  = offsetAz_deg;
        off.offsetAlt = offsetAlt_deg;
    }

    // Goto RA/DEC
    void gotoRaDec(float RA_hours, float DEC_deg,
                   float LST_hours,
                   float imuAz_deg, float imuAlt_deg)
    {
        // 1) Cible théorique
        float targetAz, targetAlt;
        m_raDecToAzAlt(RA_hours, DEC_deg, latitude_deg, LST_hours,
                       targetAz, targetAlt);

        // 2) Appliquer offsets d’alignement
        targetAz  = m_normalize360(targetAz  + off.offsetAz);
        targetAlt = targetAlt + off.offsetAlt;

        // 3) Delta vs IMU
        float dAz  = m_normalize180(targetAz  - imuAz_deg);
        float dAlt = m_normalize180(targetAlt - imuAlt_deg);

        // 4) Steps
        long stepsAz  = (long)(dAz  * stepsPerDegAz);
        long stepsAlt = (long)(dAlt * stepsPerDegAlt);

        // 5) Mouvement
        moveMotor(stepAzPin,  dirAzPin,  stepsAz);
        moveMotor(stepAltPin, dirAltPin, stepsAlt);
    }

private:
    void moveMotor(int stepPin, int dirPin, long steps, bool invertDir=false) {
        if (steps == 0) return;

        bool dir = (steps > 0);
        if (invertDir) dir = !dir;

        digitalWrite(dirPin, dir);
        long count = labs(steps);

        for (long i = 0; i < count; i++) {
            digitalWrite(stepPin, HIGH);
            delayMicroseconds(600);
            digitalWrite(stepPin, LOW);
            delayMicroseconds(600);
        }
    }
};
