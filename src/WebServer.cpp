#include "WebServer.hpp"
#include "Telescope.hpp"
#include "Batterie.hpp"
#include "GHFS.hpp"

AsyncWebServer WebServer::server(80);
GPSManager *WebServer::gpsManager = nullptr;

String menuFile = "menu.html";
String htmlMenu = "";

AsyncEventSource events("/events");

String WebServer::getHtml()
{
    if (htmlMenu.length() == 0)
    {
        File file = GHFS::open(menuFile);
        if (!file)
        {
#ifdef DEBUG
            Serial.println("Failed to open file for reading");
#endif
            return "<!DOCTYPE html> <html><head><title>Error</title></head><body>Menu.html not found!</body></html>";
        }
        htmlMenu = file.readString();
        file.close();
    }
    return htmlMenu;
}

void WebServer::setup(GPSManager *manager)
{
    gpsManager = manager;

    // Route pour la page d'accueil
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", getHtml().c_str()); });

    // Route pour les commandes de mouvement
    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("dir")) 
        {
            String dir = request->getParam("dir")->value();
            Serial.println("Commande reçue: " + dir);
            // Ici, vous pouvez appeler les fonctions de contrôle du télescope en fonction de 'dir'
            if (dir == "left")  Telescope::steps(-100,0);   // 100 pas en Azimut
            if (dir == "right") Telescope::steps(100,0);
            if (dir == "up")    Telescope::steps(0,100);    // 100 pas en Altitude
            if (dir == "down")  Telescope::steps(0,-100);

            if (dir == "stop")  Telescope::stop();     
            if (dir == "origine") Telescope::commanderMouvement(0, 0);
            if(dir == "calibrate") Telescope::calibrate();
            if(dir == "calibrateMVT") Telescope::calibrateMovement();
        }
        request->send(200, "text/plain", "OK"); });

    server.addHandler(&events);
    server.begin();
}

static bool stopped = false;

void WebServer::stop()
{
    if (!stopped)
    {
        Serial.println("Arrêt du serveur Web...");
        stopped = true;
        server.end();
    }
}

void WebServer::loop()
{
    if (stopped)
        return;

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 500)
    {
        // Calcul batterie (pont 300k/51k)
        float batt = Batterie::lireTension(); // * (300.0 + 51.0) / 51.0;

        // Récupération angles BNO (à adapter selon votre lib)
        EulerAngles angles = Telescope::getCurrentAngles();
        float accuracy = Telescope::getAccuracy(); // Précision du capteur

        // Création du message JSON pour le Web
        String json = "{";
        json += "\"yaw\":\"" + String(angles.yaw, 1) + "\",";
        json += "\"pitch\":\"" + String(angles.pitch, 1) + "\",";
        json += "\"roll\":\"" + String(angles.roll, 1) + "\",";
        json += "\"accuracy\":\"" + String(accuracy, 1) + "\",";
        json += "\"precision\":\"" + String(Telescope::getPrecision()) + "\",";
        json += "\"batt\":\"" + String(batt, 1) + "\",";
        json += "\"sats\":\"" + String(gpsManager->satellites()) + "\",";
        json += "\"deltaALT\":\"" + String(Telescope::getDeltaALT().toString()) + "\",";
        json += "\"deltaAZ\":\"" + String(Telescope::getDeltaAZ().toString()) + "\"";
        json += "}";

        // Envoi aux clients Web connectés
        events.send(json.c_str(), "data_update", millis());

        lastUpdate = millis();
    }
}
