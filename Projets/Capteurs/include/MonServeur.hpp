#pragma once

#include <ESPAsyncWebServer.h>
#include <WiFi.h>

class MonServeur
{
public:
    static void setup();
    static void send(String json);

private:
    static String getHtml();
    static AsyncWebServer server;
};
