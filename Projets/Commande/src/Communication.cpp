#include "Communication.hpp"
#include "imu.hpp"
#include "Log.hpp"

static WiFiServer tcpServer(portTCP);

CapteursMessage Communication::incomingData;
String Communication::error = "";

// ... (Garde ta structure CapteursMessage et ta variable incomingData)

CapteursMessage *Communication::getData() { return &incomingData; }

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
    tcpServer.begin();
}

int Communication::receive()
{
    int nbDataReceived = 0;
    unsigned long startTime = millis();
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
                Imu::saveIMUData(&incomingData);
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