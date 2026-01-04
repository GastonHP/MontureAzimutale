#pragma once
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
class GPSManager
{

public:
    GPSManager(int rxPin, int txPin, int baud = 9600);

    void begin();
    void update();
    bool hasFix();
    double latitude();
    double longitude();
    double altitude();
    double speedKmh();
    int satellites();

    String timeString();

private:
    int _rx, _tx, _baud;
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;
};