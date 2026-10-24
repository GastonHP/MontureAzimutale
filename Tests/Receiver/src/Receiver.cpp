#include "Receiver.hpp"
#include "Log.hpp"
#include "WiFiUdp.h"

WiFiUDP udp;

char packetBuffer[512]; // Buffer de réception

String Receiver::error = "";
void Receiver::setup(bool SSIDFound = true)
{
    if (!udp.begin(udpLocalPort))
    {
        error = "Erreur lors de l'initialisation du port UDP";
        Log::addError(error);
        return;
    }
    Log::addLog("Récepteur ESP-NOW initialisé sur le port " + String(udpLocalPort));
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
            Log::addLog("Paquet reçu de " + udp.remoteIP().toString() + " : " + String(packetBuffer));
            // Traiter les données reçues ici
            udp.beginPacket(udp.remoteIP(), udp.remotePort());
            udp.write(reinterpret_cast<uint8_t *>(packetBuffer), sizeof(len));
            udp.endPacket();
        }
    }
}
