#include "Parametres.hpp"
Parametres::GPSParam Parametres::gpsParam;

bool Parametres::read()
{
    File f = GHFS::open("/gpsparam.bin", "r");
    if (!f)
    {
        Log::addLog(F("Parametres::read: Failed to open gpsparam.bin for reading"));
        return false;
    }
    GPSParam tempParam;
    f.read((uint8_t *)&tempParam, sizeof(GPSParam));
    f.close();
    if (tempParam.magic != GPSParamMagic)
    {
        Log::addLog(F("Parametres::read: Invalid magic number in gpsparam.bin"));
        return false;
    }
    gpsParam = tempParam;
    Log::addLog(F("Parametres::read: GPS parameters read successfully"));
    return true;
}

bool Parametres::write()
{
    gpsParam.magic = GPSParamMagic;
    File f = GHFS::open("/gpsparam.bin", "w");
    if (!f)
    {
        Log::addLog(F("Parametres::write: Failed to open gpsparam.bin for writing"));
        return false;
    }
    f.write((uint8_t *)&gpsParam, sizeof(GPSParam));
    f.close();
    Log::addLog(F("Parametres::write: GPS parameters written successfully"));
    return true;
}