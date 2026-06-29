#include "MonServeur.hpp"
#include "Telescope.hpp"
#include "Batterie.hpp"
#include "GHFS.hpp"
#include "html_autogen.h"

//
AsyncWebServer MonServeur::server(80);
bool MonServeur::activated = false;

AsyncEventSource events("/events");

void MonServeur::setup()
{
    // Route pour la page d'accueil
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", INDEX_HTML); });
    // Route pour reboot
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
        request->send(200, "text/plain", "Rebooting...");
        delay(100); // Assurez-vous que la réponse est envoyée avant de redémarrer
        ESP.restart(); });
    // Route pour les commandes de mouvement
    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("dir")) 
        {
            String dir = request->getParam("dir")->value();
            Serial.println("Commande reçue: " + dir);
            // Ici, vous pouvez appeler les fonctions de contrôle du télescope en fonction de 'dir'
            if (dir == "left")  Telescope::addCommande(Telescope::CMD_Steps, 100, 0);   // 100 pas en Azimut
            if (dir == "right") Telescope::addCommande(Telescope::CMD_Steps, -100, 0);
            if (dir == "up")    Telescope::addCommande(Telescope::CMD_Steps, 0, 100);    // 100 pas en Altitude
            if (dir == "down")  Telescope::addCommande(Telescope::CMD_Steps, 0, -100);

            if (dir == "stop")  Telescope::stop();     
            if (dir == "origine") Telescope::commanderMouvement(0, 0);
            if(dir == "calibrate") Telescope::calibrate();
            if(dir == "calibrateMVT") Telescope::addCommande(Telescope::CMD_CalibrateMouvement);
            if(dir == "calibrateAZ") Telescope::addCommande(Telescope::CMD_CalibrateAZ);
            if(dir == "calibrateALT") Telescope::addCommande(Telescope::CMD_CalibrateALT);
        }
        request->send(200, "text/plain", "OK"); });

    server.addHandler(&events);
    server.begin();
    activated = true;
}

static bool stopped = false;

// void MonServeur::stop()
// {
//     if (!stopped)
//     {
//         Serial.println("Arrêt du serveur Web...");
//         stopped = true;
//         server.end();
//     }
// }

void MonServeur::setActivated(bool state)
{
    if (state == true && activated == false)
    {
        server.begin();
    }
    else if (state == false && activated == true)
    {
        server.end();
    }
    activated = state;
}

void MonServeur::loop()
{
    if (stopped || !activated)
        return;

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 1000)
    {
        // Envoi aux clients Web connectés
        events.send(Telescope::getJson().c_str(), "data_update", millis());
        lastUpdate = millis();
    }
}
