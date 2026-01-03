#include <Wire.h>
#include <HardwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <TinyGPSPlus.h>

// Déclaration du GPS sur Serial2
HardwareSerial GPS_Serial(2);

// The TinyGPSPlus object
TinyGPSPlus gps;

void gps_displayInfo()
{
    Serial.print(F("Location: "));
    if (gps.location.isValid())
    {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F("  Date/Time: "));
    if (gps.date.isValid())
    {
        Serial.print(gps.date.month());
        Serial.print(F("/"));
        Serial.print(gps.date.day());
        Serial.print(F("/"));
        Serial.print(gps.date.year());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.print(F(" "));
    if (gps.time.isValid())
    {
        if (gps.time.hour() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        if (gps.time.minute() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        if (gps.time.second() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(F("."));
        if (gps.time.centisecond() < 10)
            Serial.print(F("0"));
        Serial.print(gps.time.centisecond());
    }
    else
    {
        Serial.print(F("INVALID"));
    }

    Serial.println();
}

void gps_displayInfo(Adafruit_ST7789 *monEcranptr)
{
    String s = "Location: ";
    if (gps.location.isValid())
    {
        s += String(gps.location.lat(), 6);
        s += ",";
        s += String(gps.location.lng(), 6);
    }
    else
    {
        s += "INVALID";
    }
    monEcranptr->println(s);

    s = "  Date: ";
    if (gps.date.isValid())
    {
        s += String(gps.date.month());
        s += "/";
        s += String(gps.date.day());
        s += "/";
        s += String(gps.date.year());
    }
    else
    {
        s += "INVALID";
    }
    monEcranptr->println(s);
    
    s = "  Time: ";
    if (gps.time.isValid())
    {
        if (gps.time.hour() < 10)
            s += "0";
        s += String(gps.time.hour());
        s += ":";
        if (gps.time.minute() < 10)
            s += "0";
        s += String(gps.time.minute());
        s += ":";
        if (gps.time.second() < 10)
            s += "0";
        s += String(gps.time.second());
        s += ".";

        if (gps.time.centisecond() < 10)
            s += "0";
        s += String(gps.time.centisecond());
    }
    else
    {
        s += "INVALID";
    }
    monEcranptr->println(s);
}

void GPS_setup(Adafruit_ST7789 *monEcranptr)
{
    // 3. UART pour GPS (Vitesse standard 9600 pour Neo-6M)
    GPS_Serial.begin(9600, SERIAL_8N1, 18, 17); // RX=18, TX=17

    Serial.println("Tous les périphériques sont assignés.");
}

void GPS_loop(Adafruit_ST7789 *monEcranptr)
{
    // Lecture des données GPS
    while (GPS_Serial.available() > 0)
    {
        gps.encode(GPS_Serial.read());
        if (gps.location.isUpdated())
        {
            gps_displayInfo(monEcranptr);
            gps_displayInfo();
        }
    }
}