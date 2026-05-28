#include "GPSManager.hpp"
#include "monEcran.hpp"
#include "HardwareSerial.h"
#include <TinyGPSPlus.h>

#define GPS_RX_PIN 18
#define GPS_TX_PIN 17
#define GPS_BAUD_RATE 9600
// Séquence hexadécimale pour passer à 0.5Hz (UBX-CFG-RATE)
static const byte GPS_HZ[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xD0, 0x07, 0x01, 0x00, 0x01, 0x00, 0x57, 0x92};
// 1. Passer le GPS à 38400 bauds (Commande UBX-CFG-PRT)
static const byte packet_38400baud[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00,
    0x00, 0x96, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x90};
// Désactiver les messages inutiles (GSV = Satellites in View, très bavard)
static const char DISABLE_GSV[] = "$PUBX,40,GSV,0,0,0,0,0,0*59";
static const char DISABLE_GSA[] = "$PUBX,40,GSA,0,0,0,0,0,0*4E";
static const char DISABLE_VTG[] = "$PUBX,40,VTG,0,0,0,0,0,0*5E";

static HardwareSerial gpsSerial(2);
static TinyGPSPlus gps;
static int nFixes = 0;
static GPSManager::portStatus portStatusValue = GPSManager::Closed;

void GPSManager::setup()
{
    // Initialisation du port série pour le GPS
    delay(1000);
    gpsSerial.setRxBufferSize(2048);
    //
    gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    delay(1000);
    // gpsSerial.write(packet_38400baud, sizeof(packet_38400baud));
    // delay(1000);
    // gpsSerial.updateBaudRate(38400);
    // delay(1000);
    // 1. Envoyer la fréquence Hz
    // gpsSerial.write(GPS_HZ, sizeof(GPS_HZ));
    // delay(100);

    // 2. Désactiver les phrases inutiles pour alléger le buffer
    gpsSerial.println(DISABLE_GSV);
    delay(50);
    gpsSerial.println(DISABLE_GSA);
    delay(50);
    gpsSerial.println(DISABLE_VTG);
}

void GPSManager::stop()
{
    gpsSerial.end();
    portStatusValue = Closed;
}

void GPSManager::loop()
{
    static unsigned long nextFix = 0;
    static unsigned long nextLoop;
    if (millis() > nextFix)
    {
        nextFix = millis() + 60000;
        nFixes = 0; // reset fix count every minute
    }

    if (nFixes >= 5)
        return; // stop after 100 fixes
    if (millis() < nextLoop)
        return;

    switch (portStatusValue)
    {
    case Closed:
        nextLoop = millis() + 1000;
        gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
        portStatusValue = Open;
        break;
    case Open:
        nextLoop = millis() + 1000;
        update();
        gpsSerial.end();
        portStatusValue = Closed;
        if (hasFix())
            nFixes++;
        break;
    }
}

void GPSManager::update()
{
    static int ligne = 0;
    static int colonne = 0;
    unsigned long start = millis();
    // Log::addLog("GPSManager::update called. available=" + String(gpsSerial.available()));
    if (gpsSerial.available() > 0)
    {
        int n = 0;
        //     Serial.print("\n\r-->" + String(millis(), DEC) + ":" + String(gpsSerial.available(), DEC) + " bytes available : " + String(gpsSerial.baudRate(), DEC) + " bps : ");
        while (gpsSerial.available() > 0) //|| (millis() - start < 500))
        {
            if (gpsSerial.available() > 0)
            {
                int r = gpsSerial.read();
                n++;
                bool rv = gps.encode(r);
#ifdef DEBUG_GPS
                if (rv)
                    if (r == '$')
                    {
                        colonne = 0;
                        ligne += 20;
                        if (ligne > 300)
                            ligne = 0;
                        monEcranptr->setCursor(colonne, ligne);
                        monEcranptr->setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
                    }
                if (colonne < 240 && rv == false)
                {
                    colonne += 8;
                    monEcranptr->print((char)r);
                }

// delay(10); // to allow serial buffer to fill
#endif
                // Serial.print((char)r);
                delay(2); // to allow serial buffer to fill
            }
        }
        // Serial.print("<- " + String(n, DEC) + " ->");
    }
}

bool GPSManager::hasFix() { return gps.location.isValid() && gps.location.age() < 2000; }
double GPSManager::latitude() { return gps.location.lat(); }
double GPSManager::longitude() { return gps.location.lng(); }
double GPSManager::altitude() { return gps.altitude.meters(); }
double GPSManager::speedKmh() { return gps.speed.kmph(); }
int GPSManager::satellites() { return gps.satellites.value(); }

String GPSManager::timeString()
{
    if (!gps.time.isValid())
        return "----";
    char buffer[16];
    sprintf(buffer, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    return String(buffer);
}
