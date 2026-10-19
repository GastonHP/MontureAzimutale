#include "Sender.hpp"
#include "Log.hpp"
#include "WiFiUdp.h"

WiFiUDP udp;
const unsigned int localPort = 4210;  // Port d'écoute local
char packetBuffer[512];               // Buffer de réception
IPAddress targetIP(192, 168, 0, 179); // Adresse IP du récepteur

bool Sender::openConnection()
{
    // Log::addLog("Sender::openConnection()", true);
    return true;
}

bool Sender::closeConnection()
{
    // Log::addLog("Sender::closeConnection()", true);
    return true;
}

void Sender::setup(bool networkHP = true)
{
    if (!udp.begin(localPort))
    {
        String error = "Erreur lors de l'initialisation du port UDP";
        Log::addError(error);
        return;
    }
}

bool Sender::send(MessageData *incomingData)
{
    static int messageCounter = 0;
    messageCounter++;
    incomingData->mon_timestamp = millis();
    incomingData->sent = false;              // Marquer le message comme non envoyé avant l'envoi
    incomingData->treated = false;           // Marquer le message comme non traité avant l'envoi
    incomingData->received = false;          // Marquer le message comme non reçu avant l'envoi
    incomingData->version = protocolVersion; // Assurez-vous que la version est correctement définie
    incomingData->number = messageCounter;   // Assurez-vous que le numéro de message est correctement défini
    Log::addLog("Sender::send()", true);
    udp.beginPacket(targetIP, localPort);
    udp.write(reinterpret_cast<uint8_t *>(incomingData), sizeof(MessageData));
    udp.endPacket();
    Log::addLog("Sender::send() ended", true);
    return true;
}

bool Sender::loop()
{
    // Log::addLog("Sender::loop()", true);
    //  2. --- ENVOI (toutes les 2 secondes par exemple) ---
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 2000)
    {
        lastSend = millis();

        udp.beginPacket(targetIP, localPort);
        udp.print("Hello depuis l'IP " + WiFi.localIP().toString());
        udp.endPacket();

        Serial.println("Message UDP envoyé à " + targetIP.toString());
    }
    return true;
}
