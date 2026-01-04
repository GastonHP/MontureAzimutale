#include "GPSManager.hpp"

GPSManager::GPSManager(int rxPin, int txPin, int baud) : _rx(rxPin), _tx(txPin), _baud(baud), gpsSerial(2) {}

void GPSManager::begin() { gpsSerial.begin(_baud, SERIAL_8N1, _rx, _tx); }
void GPSManager::update()
{
    while (gpsSerial.available())
        gps.encode(gpsSerial.read());
}

bool GPSManager::hasFix()  { return gps.location.isValid() && gps.location.age() < 2000; }
double GPSManager::latitude()  { return gps.location.lat(); }
double GPSManager::longitude()  { return gps.location.lng(); }
double GPSManager::altitude()  { return gps.altitude.meters(); }
double GPSManager::speedKmh()  { return gps.speed.kmph(); }
int GPSManager::satellites()  { return gps.satellites.value(); }

String GPSManager::timeString() 
{
    if (!gps.time.isValid())
        return "----";
    char buffer[16];
    sprintf(buffer, "%02d:%02d:%02d",
            gps.time.hour(),
            gps.time.minute(),
            gps.time.second());
    return String(buffer);
}
