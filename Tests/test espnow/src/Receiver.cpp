#include "Receiver.hpp"
#include "capteur.hpp"
#include "Log.hpp"
#include "WiFiUdp.h"

WiFiUDP udp;
const unsigned int localPort = 4210; // Port d'écoute local
char packetBuffer[512];              // Buffer de réception

String Receiver::error = "";
void Receiver::setup(bool SSIDFound = true)
{
    if (!udp.begin(localPort))
    {
        error = "Erreur lors de l'initialisation du port UDP";
        Log::addError(error);
        return;
    }
    Log::addLog("Récepteur ESP-NOW initialisé sur le port " + String(localPort));
}

void Receiver::loop()
{
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
        // Lire le paquet reçu
        int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
        if (len > 0)
        {
            packetBuffer[len] = '\0'; // Terminer la chaîne de caractères
            Log::addLog("Paquet reçu : " + String(packetBuffer));
            // Traiter les données reçues ici
        }
    }
}
