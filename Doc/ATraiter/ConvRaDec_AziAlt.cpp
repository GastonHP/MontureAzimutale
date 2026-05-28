// float RA = 6.7525;     // Sirius (exemple)
// float DEC = -16.70;

// float latitude = 50.74;   // Mouscron
// float LST = 12.345;       // Temps sidéral local (heures)

// float Az, Alt;
// raDecToAzAlt(RA, DEC, latitude, LST, Az, Alt);

// Serial.printf("Azimut = %.2f°, Altitude = %.2f°\n", Az, Alt);






#include <math.h>

// ------------------------------
// Outils math
// ------------------------------
float degToRad(float d) { return d * M_PI / 180.0; }
float radToDeg(float r) { return r * 180.0 / M_PI; }

// Normalisation angle 0–360°
float normalize360(float a) {
    while (a < 0)   a += 360.0;
    while (a >= 360) a -= 360.0;
    return a;
}

// Normalisation -180 à +180
float normalize180(float a) {
    while (a < -180) a += 360.0;
    while (a > 180)  a -= 360.0;
    return a;
}

// ------------------------------
// Conversion RA/DEC → Az/Alt
// RA en heures décimales
// DEC, lat, lon en degrés
// LST en heures décimales
// ------------------------------
void raDecToAzAlt(float RA_hours, float DEC_deg, float lat_deg, float LST_hours,
                  float &Az_deg, float &Alt_deg)
{
    // 1) Convertir en radians
    float lat = degToRad(lat_deg);
    float dec = degToRad(DEC_deg);

    // 2) Angle horaire (HA)
    float HA_hours = LST_hours - RA_hours;
    float HA = degToRad(HA_hours * 15.0);  // 1h = 15°

    // 3) Altitude
    float sinAlt = sin(dec)*sin(lat) + cos(dec)*cos(lat)*cos(HA);
    float Alt = asin(sinAlt);

    // 4) Azimut
    float cosAz = (sin(dec) - sin(Alt)*sin(lat)) / (cos(Alt)*cos(lat));
    float Az = acos(cosAz);

    // Déterminer le bon quadrant
    if (sin(HA) > 0)
        Az = 2*M_PI - Az;

    // 5) Convertir en degrés
    Az_deg  = normalize360(radToDeg(Az));
    Alt_deg = radToDeg(Alt);
}
