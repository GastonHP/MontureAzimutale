#include "Communication.hpp"
#include "Log.hpp"
#include "WiFiUdp.h"
#include "Message.hpp"

WiFiUDP udp;
const unsigned int localPort = 4210;  // Port d'écoute local
IPAddress targetIP(192, 168, 0, 179); // Adresse IP du récepteur
MessageData Communication::buffer;
// bool Communication::openConnection()
// {
//     // Log::addLog("Communication::openConnection()", true);
//     return true;
// }

// bool Communication::closeConnection()
// {
//     // Log::addLog("Communication::closeConnection()", true);
//     return true;
// }

void Communication::setup(bool networkHP = true)
{
    if (!udp.begin(localPort))
    {
        String error = "Erreur lors de l'initialisation du port UDP";
        Log::addError(error);
        return;
    }
}

bool Communication::send(MessageData *messageData)
{
    static int messageCounter = 0;
    messageCounter++;
    switch (messageData->messageType)
    {
    case IMU_ROTATION_VECTOR:
        strncpy(messageData->header, "IMU_ROTATION_VECTOR", sizeof(messageData->header));
        Log::addLog("Communication::send() IMU_ROTATION_VECTOR", true);
        break;
    case IMU_ARVR_STABILIZED_RV:
        strncpy(messageData->header, "IMU_ARVR_STABILIZED_RV", sizeof(messageData->header));
        Log::addLog("Communication::send() IMU_ARVR_STABILIZED_RV", true);
        break;
    case IMU_GAME_ROTATION_VECTOR:
        strncpy(messageData->header, "IMU_GAME_ROTATION_VECTOR", sizeof(messageData->header));
        Log::addLog("Communication::send() IMU_GAME_ROTATION_VECTOR", true);
        break;
    case GPS_DATA:
        strncpy(messageData->header, "GPS_DATA", sizeof(messageData->header));
        Log::addLog("Communication::send() GPS_DATA", true);
        break;
    case STILL_ALIVE:
        strncpy(messageData->header, "STILL_ALIVE", sizeof(messageData->header));
        Log::addLog("Communication::send() STILL_ALIVE", true);
        break;
    case STRING_256:
        strncpy(messageData->header, "STRING_256", sizeof(messageData->header));
        Log::addLog("Communication::send() STRING_256", true);
        break;
    case IMU_GRAVITY:
        strncpy(messageData->header, "IMU_GRAVITY", sizeof(messageData->header));
        Log::addLog("Communication::send() IMU_GRAVITY", true);
        break;
    case ACK:
        strncpy(messageData->header, "ACK", sizeof(messageData->header));
        Log::addLog("Communication::send() ACK", true);
        break;
    case SEARCH_CLIENT:
        strncpy(messageData->header, "SEARCH_CLIENT", sizeof(messageData->header));
        Log::addLog("Communication::send() SEARCH_CLIENT", true);
        break;
    default:
        strncpy(messageData->header, "UNKNOWN", sizeof(messageData->header));
        Log::addLog("Communication::send() UNKNOWN TYPE: " + String(messageData->messageType, DEC), true);
        break;
    }
    messageData->mon_timestamp = millis();
    // Marquer le message comme non envoyé avant l'envoi
    messageData->version = protocolVersion; // Assurez-vous que la version est correctement définie
    messageData->number = messageCounter;   // Assurez-vous que le numéro de message est correctement défini
    Log::addLog("Communication::send()", true);
    udp.beginPacket(targetIP, localPort);
    udp.write(reinterpret_cast<uint8_t *>(messageData), sizeof(MessageData));
    udp.endPacket();
    Log::addLog("Communication::send() ended");
    messageData->status = Sent;
    return true;
}

unsigned long nextSend = 0;
bool Communication::loop()
{
    if (millis() > nextSend)
    {
        MessageData *m = Message::getMessageToSend();
        while (m != nullptr)
        {
            send(m);
            m = Message::getMessageToSend();
        }
        nextSend = millis() + 100;
    }
    int packetSize = udp.parsePacket();
    if (packetSize)
    {
        // Lire le paquet reçu
        int len = udp.read((char *)&buffer, sizeof(MessageData));
        if (len > 0)
        {
            // Traiter les données reçues ici
            if (buffer.version == protocolVersion)
            {
                Message::addReceivedMessage(&buffer);
                Log::addLog("Paquet reçu de " + udp.remoteIP().toString() + " : " + String(buffer.header));
            }
        }
    }
    // Log::addLog("Communication::loop()", true);
    //  2. --- ENVOI (toutes les 2 secondes par exemple) ---
    // static unsigned long lastSend = 0;
    // if (millis() - lastSend > 2000)
    // {
    //     lastSend = millis();

    //     udp.beginPacket(targetIP, localPort);
    //     udp.print("Hello depuis l'IP " + WiFi.localIP().toString());
    //     udp.endPacket();

    //     Serial.println("Message UDP envoyé à " + targetIP.toString());
    // }
    return true;
}
