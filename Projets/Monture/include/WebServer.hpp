#pragma once

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

class WebServer
{
public:
    static void setup();
    static void loop();
    static void setActivated(bool state) ;

private:
    static String getHtml();
    static AsyncWebServer server;
    static bool activated;
};
