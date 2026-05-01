#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "GPSManager.hpp"

class WebServer
{
public:
    static void setup(GPSManager *manager);
    static void loop();
    static void stop();

private:
    static String getHtml();
    static AsyncWebServer server;
    static GPSManager *gpsManager;
};

#endif // WEBSERVER_HPP
