// #include "Receiver.hpp"
// #include "Message.hpp"
// #include "Log.hpp"

// #include "WiFiUdp.h"

// WiFiUDP udp;
// MessageData Receiver::incomingData;

// String Receiver::error = "";
// void Receiver::setup(bool networkHP = true)
// {
//     if (!udp.begin(udpLocalPort))
//     {
//         error = "Erreur lors de l'initialisation du port UDP";
//         Log::addError(error);
//         return;
//     }
//     Log::addLog("Récepteur ESP-NOW initialisé sur le port " + String(udpLocalPort));
// }

// void Receiver::loop()
// {
//     int packetSize = udp.parsePacket();
//     if (packetSize)
//     {
//         // Lire le paquet reçu
//         int len = udp.read((char *)&incomingData, sizeof(MessageData));
//         if (len > 0)
//         {
//             Log::addLog("Paquet reçu : " + String(incomingData.header));
//             // Traiter les données reçues ici
//             if (incomingData.version == protocolVersion)
//                 Message::addMessage(&incomingData);
//         }
//     }
// }