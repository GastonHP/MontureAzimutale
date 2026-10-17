#include "Sender.hpp"
#include "Log.hpp"
#include "WiFiUdp.h"

WiFiUDP udp;
const unsigned int localPort = 4210; // Port d'écoute local
char packetBuffer[512];              // Buffer de réception
IPAddress targetIP(192, 168, 0, 179); // Adresse IP du récepteur

bool Sender::openConnection()
{
    //Log::addLog("Sender::openConnection()", true);
    return true;
}

bool Sender::closeConnection()
{
    //Log::addLog("Sender::closeConnection()", true);
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

bool Sender::send(CapteursMessage *incomingData)
{
    Log::addLog("Sender::send()", true);
    return true;
}

bool Sender::sendString(String str)
{
    Log::addLog("Sender::sendString()", true);
    return true;
}

bool Sender::loop()
{
    //Log::addLog("Sender::loop()", true);
    // 2. --- ENVOI (toutes les 2 secondes par exemple) ---
    static unsigned long lastSend = 0;
    if (millis() - lastSend > 2000) {
        lastSend = millis();
        
        udp.beginPacket(targetIP, localPort);
        udp.print("Hello depuis l'IP " + WiFi.localIP().toString());
        udp.endPacket();
        
        Serial.println("Message UDP envoyé à " + targetIP.toString());
    }
    return true;
}
