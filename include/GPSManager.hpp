#pragma once
#include "arduino.h"

class GPSManager
{

public:
    static void setup();
    static void loop();
    static void update();
    static bool hasFix();
    static double latitude();
    static double longitude();
    static double altitude();
    static double speedKmh();
    static int satellites();
    static void configureGPS();
    static String timeString();
    static void stop();

    enum portStatus
    {
        Open,
        Closed
    };
};