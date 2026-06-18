#pragma once
#include <TinyGPSPlus.h>
#include "commun.hpp"

class Parametres
{

public:
    static bool read();
    static bool write();

private:
    static const uint64_t GPSParamMagic = 0x504152414D455447; //"PARAMETG"
    struct GPSParam
    {
        uint64_t magic;
        double latitude = 0.0;
        double longitude = 0.0;
        double altitude = 0.0;
        float speed = 0.0;
        float course = 0.0;
        String date = "";
        String time = "";
        int satellites = 0;
        bool valid = false;
    };
    static GPSParam gpsParam;
};