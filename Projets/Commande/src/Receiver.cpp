#include "Receiver.hpp"
#include "capteur.hpp"
#include "Log.hpp"

#include "WiFiUdp.h"

WiFiUDP udp;
char packetBuffer[sizeof(MessageData)+5]; // Buffer de réception

bool sent = true;
void dataSent(uint8_t *address, uint8_t status)
{
    sent = true;
    Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
}

void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
    Capteur::saveIMUData((MessageData *)data);
    // Serial.print("Received: ");
    // Serial.printf("%.*s\n", len, data);
    // Serial.printf("RSSI: %d dBm\n", rssi);
    // Serial.printf("From: " MACSTR "\n", MAC2STR(address));
    // Serial.printf("%s\n", broadcast ? "Broadcast" : "Unicast");
}

String Receiver::error = "";
void Receiver::setup(bool networkHP = true)
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
            Log::addLog("Paquet reçu : " + String(packetBuffer));
            // Traiter les données reçues ici
        }
    }
}