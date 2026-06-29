#include "Communication.hpp"

// Variables dynamiques qui changeront selon le lieu
String targetIP = "";
WiFiClient clientCapteur;

// ... (Garde ta structure IMUData et imuData)

void Communication::setup(bool networkHP = true)
{
    if (networkHP)
    {
        // 🏠 À LA MAISON
        Serial.println("\n🏠 Connecté au Wi-Fi maison !");
        // ⚠️ Remplplace ici par l'IP que ton S3 affiche quand il est connecté à ta box !
        targetIP = "192.168.0.228"; // ou "192.168.0.227"
    }
    else
    {
        // 🌌 À L'EXTÉRIEUR
        Serial.println("\n❌ Wi-Fi maison introuvable. Connexion au S3 Nomade...");
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        WiFi.begin(ap_ssid, ap_password);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\n✅ Connecté directement à l'ESP32-S3 !");

        // En mode AP, l'IP du serveur (le S3) est TOUJOURS 192.168.4.1
        targetIP = "192.168.4.1";
    }
}
unsigned long nextTime = 0;
bool Communication::send(IMUData *incomingData)
{
    // Le reste du code du loop() TCP précédent reste EXACTEMENT le même !
    if (!clientCapteur.connected() && millis() < nextTime)
        return false;
    nextTime = millis() + 10000;
    if (!clientCapteur.connected())
    {
        if (clientCapteur.connect(targetIP.c_str(), portTCP))
        {
            Serial.println("🤝 Connecté au S3 !");
        }
        else
        {
            Serial.println("❌ NON Connecté au S3 !");
            return false;
        }
    }

    clientCapteur.write((uint8_t *)incomingData, sizeof(IMUData));
    clientCapteur.flush();
    return true;
}