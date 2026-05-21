#pragma once
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "Log.hpp"
#include "commun.hpp"

class GPSManager
{

public:
    GPSManager(int rxPin, int txPin, int baud = 9600);

    void setup();
    void loop();
    void update();
    bool hasFix();
    double latitude();
    double longitude();
    double altitude();
    double speedKmh();
    int satellites();
    void configureGPS();
    String timeString();
    void stop();
  
private:
    int _rx, _tx, _baud;
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;

    unsigned long nextLoop = 0;
    int nFixes = 0;
    enum portStatus
    {
        Open,
        Closed
    };
    portStatus portStatusValue = Closed;
};