#pragma once

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

class MonServeur
{
public:
    static void setup();
    static void loop();
    static void setActivated(bool state) ;

private:
    static AsyncWebServer server;
    static bool activated;
};
