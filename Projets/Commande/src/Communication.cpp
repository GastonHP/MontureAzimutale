#include "Communication.hpp"
#include "capteur.hpp"
#include "Log.hpp"

#ifdef MODE_TCP
static WiFiServer tcpServer(portTCP);
CapteursMessage Communication::incomingData;
CapteursMessage *Communication::getData() { return &incomingData; }
#endif

#ifdef MODE_MAC
#include <QuickEspNow.h>
bool sent = true;
void dataSent(uint8_t *address, uint8_t status)
{
    sent = true;
    Serial.printf("Message sent to " MACSTR ", status: %d\n", MAC2STR(address), status);
}

void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
    Capteur::saveIMUData((CapteursMessage *)data);
    // Serial.print("Received: ");
    // Serial.printf("%.*s\n", len, data);
    // Serial.printf("RSSI: %d dBm\n", rssi);
    // Serial.printf("From: " MACSTR "\n", MAC2STR(address));
    // Serial.printf("%s\n", broadcast ? "Broadcast" : "Unicast");
}
#endif

String Communication::error = "";
void Communication::setup(bool SSIDFound = true)
{
    if (SSIDFound)
    {
        // 🏠 MODE MAISON
        Serial.print("\n🏠 Mode Maison activé ! IP du S3 : ");
        Log::addLog("🏠 Mode Maison activé ! IP du S3 : " + WiFi.localIP().toString());
        Serial.println(WiFi.localIP());
    }
    else
    {
        // 🌌 MODE EXTÉRIEUR (Nomade)
        WiFi.mode(WIFI_AP_STA); // Mode mixte : peut chercher un wifi ET en créer un
        Serial.println("\n❌ Wi-Fi maison introuvable. Passage en MODE NOMADE...");
        WiFi.disconnect();

        // Le S3 crée son propre Wi-Fi sur le Canal 1
        WiFi.softAP(ap_ssid, ap_password, 1);

        Serial.print("🌌 Réseau nomade créé : ");
        Serial.println(ap_ssid);
        Serial.print("👉 IP fixe du S3 en mode nomade : ");
        Serial.println(WiFi.softAPIP()); // Ce sera TOUJOURS 192.168.4.1
    }
#ifdef MODE_TCP
    tcpServer.begin();
#endif
#ifdef MODE_MAC
    quickEspNow.begin(1, 0, false);
    quickEspNow.onDataSent(dataSent);
    quickEspNow.onDataRcvd(dataReceived);
#endif
}

#ifdef MODE_TCP
unsigned long startTime = millis();
int Communication::receive()
{
    int nbDataReceived = 0;

    WiFiClient clientS3 = tcpServer.available();
    if (!clientS3)
    {
        Communication::error = "No client connected.";
        return 0;
    }
    Log::addLog("Client connected from " + clientS3.remoteIP().toString() + ":" + String(clientS3.remotePort()));
    // Reste dans la boucle tant que le client est connecté et envoie des données
    while (clientS3.connected())
    {
        while (clientS3.available())
        {
            clientS3.read((uint8_t *)&incomingData, sizeof(incomingData));
            if (incomingData.version != protocolVersion)
            {
                Communication::error = "Version mismatch: expected " + String(protocolVersion) + ", got " + String(incomingData.version);
            }
            else
            {
                incomingData.mon_timestamp = millis();
                Capteur::saveIMUData(&incomingData);
                Communication::error = "Data received successfully.";
                // Serial.printf("Quaternions R: %.4f\n", incomingData.q_real);
                nbDataReceived++;
            }
        }
    }

    // Le client s'est déconnecté (ou a envoyé client.stop())
    clientS3.stop(); // LIBÈRE LA SOCKET CÔTÉ SERVEUR
    Serial.println("Socket libéré côté serveur");
    Log::addLog("temps écoulé: " + String(millis() - startTime) + " ms - nbDataReceived: " + String(nbDataReceived) + " - Client disconnected from " + clientS3.remoteIP().toString() + ":" + String(clientS3.remotePort()));

    return nbDataReceived;
}
#endif