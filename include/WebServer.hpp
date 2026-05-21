#pragma once

#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "GPSManager.hpp"

class WebServer
{
public:
    static void setup(GPSManager *manager);
    static void loop();
    static void stop();
    static void setActivated(bool state) { activated = state; }

private:
    static String getHtml();
    static AsyncWebServer server;
    static GPSManager *gpsManager;
    static bool activated;
};
