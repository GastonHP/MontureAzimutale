#include <math.h>

// -----------------------------------------------------
// OUTILS ANGLES
// -----------------------------------------------------
float degToRad(float d) { return d * M_PI / 180.0; }
float radToDeg(float r) { return r * 180.0 / M_PI; }

float normalize360(float a) {
    while (a < 0) a += 360.0;
    while (a >= 360) a -= 360.0;
    return a;
}

float normalize180(float a) {
    while (a < -180) a += 360.0;
    while (a > 180) a -= 360.0;
    return a;
}

// -----------------------------------------------------
// RA/DEC → AZ/ALT
// -----------------------------------------------------
void raDecToAzAlt(float RA_hours, float DEC_deg, float lat_deg, float LST_hours,
                  float &Az_deg, float &Alt_deg)
{
    float lat = degToRad(lat_deg);
    float dec = degToRad(DEC_deg);

    float HA_hours = LST_hours - RA_hours;
    float HA = degToRad(HA_hours * 15.0);

    float sinAlt = sin(dec)*sin(lat) + cos(dec)*cos(lat)*cos(HA);
    float Alt = asin(sinAlt);

    float cosAz = (sin(dec) - sin(Alt)*sin(lat)) / (cos(Alt)*cos(lat));
    float Az = acos(cosAz);

    if (sin(HA) > 0)
        Az = 2*M_PI - Az;

    Az_deg  = normalize360(radToDeg(Az));
    Alt_deg = radToDeg(Alt);
}

// -----------------------------------------------------
// MOTEURS DRV8825
// -----------------------------------------------------
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

// -----------------------------------------------------
// GOTO COMPLET
// -----------------------------------------------------
void gotoStar(float RA_hours, float DEC_deg,
              float latitude_deg, float LST_hours,
              float imuAz, float imuAlt,
              float stepsPerDegAz, float stepsPerDegAlt,
              int stepAzPin, int dirAzPin,
              int stepAltPin, int dirAltPin)
{
    // 1) Calcul cible
    float targetAz, targetAlt;
    raDecToAzAlt(RA_hours, DEC_deg, latitude_deg, LST_hours, targetAz, targetAlt);

    // 2) Delta
    float dAz  = normalize180(targetAz  - imuAz);
    float dAlt = normalize180(targetAlt - imuAlt);

    // 3) Steps
    long stepsAz  = dAz  * stepsPerDegAz;
    long stepsAlt = dAlt * stepsPerDegAlt;

    // 4) Déplacement
    moveMotor(stepAzPin,  dirAzPin,  stepsAz);
    moveMotor(stepAltPin, dirAltPin, stepsAlt);
}
