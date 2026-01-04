#include <Wire.h>
#include <HardwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <TinyGPSPlus.h>
#include "GPSManager.hpp"

GPSManager gpsManager(18, 17, 9600); // RX=18, TX=17

unsigned long lastDisplay = 0;
const unsigned long DISPLAY_INTERVAL = 1000; // Intervalle d'affichage en millisecond

void GPS_setup(Adafruit_ST7789 *monEcranptr)
{
    gpsManager.begin();
    Serial.println("Tous les périphériques sont assignés.");
}

void GPS_loop(Adafruit_ST7789 *monEcranptr)
{
    gpsManager.update();
    if (gpsManager.hasFix())
    {
        monEcranptr->setCursor(0, 5);
        monEcranptr->printf("Lat:   %7.2f\n\r", gpsManager.latitude());
        monEcranptr->printf("Lng:   %7.2f\n\r", gpsManager.longitude());
        monEcranptr->printf("Alt:   %.2f m\n\r", gpsManager.altitude());
        monEcranptr->printf("Vitesse: %.2f km/h\n\r", gpsManager.speedKmh());
        monEcranptr->printf("Satellites: %d\n\r", gpsManager.satellites());
        monEcranptr->printf("Heure: %s\n\r", gpsManager.timeString().c_str());
    }
    else
    {
        Serial.println("Recherche satellites...");
    }
}