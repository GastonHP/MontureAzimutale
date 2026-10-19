#include "Arduino.h"

void printInfo()
{
  Serial.println("");
  Serial.println("Date compilation : " __DATE__ " " __TIME__);
  Serial.println("Time size        : " + String(sizeof(time_t), DEC));
  Serial.println("##################################");
#ifdef ESP32
  // Initialisation

  if (psramInit())
    Serial.println("PSRam             = OK");
  else
    Serial.println("PSRam             = ne fonctionne pas!");
  Serial.println("ChipCores         = " + String(ESP.getChipCores(), DEC));
  Serial.println("ChipModel         = " + String(ESP.getChipModel()));
  Serial.println("ChipRevision      = " + String(ESP.getChipRevision(), DEC));
  Serial.println("CpuFreqMHz        = " + String(ESP.getCpuFreqMHz(), 10));
  Serial.println("CycleCount        = " + String(ESP.getCycleCount(), DEC));
  Serial.println("EfuseMac          = " + String(ESP.getEfuseMac(), HEX));
  Serial.println("FlashChipMode     = " + String(ESP.getFlashChipMode(), 10));
  Serial.println("FlashChipSpeed    = " + String(ESP.getFlashChipSpeed(), 10));
  Serial.println("FlashChipSize     = " + String(ESP.getFlashChipSize(), 10));

  Serial.println("HeapSize          = " + String(ESP.getHeapSize(), 10));
  Serial.println("FreeHeap          = " + String(ESP.getFreeHeap(), 10));
  Serial.println("MinFreeHeap       = " + String(ESP.getMinFreeHeap(), 10));
  Serial.println("MaxAllocHeap      = " + String(ESP.getMaxAllocHeap(), 10));

  Serial.println("PsramSize         = " + String(ESP.getPsramSize(), 10));
  Serial.println("FreePsram         = " + String(ESP.getFreePsram(), 10));
  Serial.println("MaxAllocPsram     = " + String(ESP.getMaxAllocPsram(), 10));
  Serial.println("MinFreePsram      = " + String(ESP.getMinFreePsram(), 10));

  Serial.println("FreeSketchSpace   = " + String(ESP.getFreeSketchSpace(), 10));
  Serial.println("SdkVersion        = " + String(ESP.getSdkVersion()));
  Serial.println("SketchMD5         = " + ESP.getSketchMD5());
  Serial.println("SketchSize        = " + String(ESP.getSketchSize(), 10));

#else
  Serial.println("ESP ChipID = " + String(ESP.getChipId(), 16));
  Serial.println("ESP Boot Mode = " + String(ESP.getBootMode(), 16));
  Serial.println("ESP Boot Version = " + String(ESP.getBootVersion(), 16));
  Serial.println("ESP Core Version = " + ESP.getCoreVersion());
  Serial.println("ESP Cpu Freq MHz = " + String(ESP.getCpuFreqMHz(), 10));
  Serial.println("ESP Flash ChipID = " + String(ESP.getFlashChipId(), 16));
  Serial.println("ESP Flash Chip Mode = " + String(ESP.getFlashChipMode(), 10));
  Serial.println("ESP Flash Chip Speed = " + String(ESP.getFlashChipSpeed(), 10));
  Serial.println("ESP Flash Chip Size = " + String(ESP.getFlashChipSize(), 10));
  Serial.println("ESP Flash Chip Real Size = " + String(ESP.getFlashChipRealSize(), 10));
  Serial.println("ESP Free Heap = " + String(ESP.getFreeHeap(), 10));
  Serial.println("ESP Free Sketch Space = " + String(ESP.getFreeSketchSpace(), 10));
  Serial.println("ESP Full Version = " + ESP.getFullVersion());
  Serial.println("ESP Sketch Size = " + String(ESP.getSketchSize(), 10));
  Serial.println("ESP Free Cont Stack = " + String(ESP.getFreeContStack(), 10));
  Serial.println("ESP VCC = " + String(ESP.getVcc(), 10));
#endif
  // Serial.println("ESP MAC Address = " + WiFi.macAddress());
  Serial.println("##################################");
}