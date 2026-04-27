#include "GPSManager.hpp"

// Séquence hexadécimale pour passer à 0.5Hz (UBX-CFG-RATE)
const byte GPS_HZ[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xD0, 0x07, 0x01, 0x00, 0x01, 0x00, 0x57, 0x92};
// 1. Passer le GPS à 38400 bauds (Commande UBX-CFG-PRT)
byte packet_38400baud[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00,
    0x00, 0x96, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x90};
// Désactiver les messages inutiles (GSV = Satellites in View, très bavard)
const char DISABLE_GSV[] = "$PUBX,40,GSV,0,0,0,0,0,0*59";
const char DISABLE_GSA[] = "$PUBX,40,GSA,0,0,0,0,0,0*4E";
const char DISABLE_VTG[] = "$PUBX,40,VTG,0,0,0,0,0,0*5E";

GPSManager::GPSManager(int rxPin, int txPin, int baud) : _rx(rxPin), _tx(txPin), _baud(baud), gpsSerial(2) {}

void GPSManager::setup(Adafruit_ST7789 *monEcranptr)
{
    Ecranptr = monEcranptr;
    // Initialisation du port série pour le GPS
    delay(1000);
    gpsSerial.setRxBufferSize(2048);
    //
    gpsSerial.begin(_baud, SERIAL_8N1, _rx, _tx);
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

unsigned long nextFix = 0;

void GPSManager::loop()
{
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
        gpsSerial.begin(_baud, SERIAL_8N1, _rx, _tx);
        portStatusValue = Open;
        break;
    case Open:
        nextLoop = millis() + 1000;
        update();
        gpsSerial.end();
        portStatusValue = Closed;
        if (hasFix())
        {
            nFixes++;
            if (Ecranptr != nullptr)
            {
                Ecranptr->setCursor(0, 0);
                Ecranptr->setTextColor(ST77XX_CYAN, ST77XX_BLACK);
                Ecranptr->printf("UT: %s Sat:%d\n\r", timeString().c_str(), satellites());
                Ecranptr->printf("%05.2fN   %05.2fE\n\r", latitude(), longitude());
                // Ecranptr->printf("Alt:%.2fm Sat:%d\n\r", altitude(), satellites());
                // Ecranptr->printf("Vitesse: %.2f km/h\n\r", speedKmh());
            }
        }
        else
        {
            // Serial.println("Recherche satellites...");
            // monEcranptr->setCursor(0, 0);
            // monEcranptr->printf("Recherche satellites...\n\r");
        }
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
                //Serial.print((char)r);
                delay(2); // to allow serial buffer to fill
            }
        }
        //Serial.print("<- " + String(n, DEC) + " ->");
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
