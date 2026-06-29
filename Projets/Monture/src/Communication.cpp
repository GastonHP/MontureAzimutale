#include "Communication.hpp"
#include "imu.hpp"

static WiFiServer tcpServer(portTCP);
static WiFiClient clientS3;

IMUData Communication::incomingData;

// ... (Garde ta structure IMUData et ta variable incomingData)

IMUData *Communication::getData() { return &incomingData; }

void Communication::setup(bool SSIDFound = true)
{
    if (SSIDFound)
    {
        // 🏠 MODE MAISON
        Serial.print("\n🏠 Mode Maison activé ! IP du S3 : ");
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

bool Communication::receive()
{
    if (!clientS3 || !clientS3.connected())
    {
        clientS3 = tcpServer.available();
    }
    if (clientS3 && clientS3.connected() && clientS3.available() >= sizeof(IMUData))
    {
        clientS3.read((uint8_t *)&incomingData, sizeof(incomingData));
        Imu::addMessage(&incomingData);
        //Serial.printf("Quaternions R: %.4f\n", incomingData.q_real);
        return true;
    }
    return false;
}