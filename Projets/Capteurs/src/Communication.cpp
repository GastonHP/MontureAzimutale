#include "Communication.hpp"
#include "Log.hpp"

// Variables dynamiques qui changeront selon le lieu
// IPAddress targetIP;
String targetIP = "";

// ... (Garde ta structure IMUData et imuData)

void Communication::setup(bool networkHP = true)
{
    if (networkHP)
    {
        // 🏠 À LA MAISON
        Serial.println("\n🏠 Connecté au Wi-Fi maison !");
        Log::addLog("🏠 Connecté au Wi-Fi maison ! IP du S3 : " + WiFi.localIP().toString());
        // ⚠️ Remplplace ici par l'IP que ton S3 affiche quand il est connecté à ta box !
        targetIP = "192.168.0.228";
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
bool Communication::send(CapteursMessage *incomingData)
{
    WiFiClient clientCapteur;
    Log::addLog("communication::send()");

    if (millis() < nextTime)
        return false;
    nextTime = millis() + 1000;

    if (!clientCapteur.connect(targetIP.c_str(), portTCP))
    {
        Serial.println("❌ NON Connecté au S3 !");
        Log::addLog("❌ NON Connecté au S3 !");
        return false;
    }
    Serial.println("🤝 Connecté au S3 !");
    Log::addLog("🤝 Connecté au S3 !");
    clientCapteur.write((uint8_t *)incomingData, sizeof(CapteursMessage));
    Log::addLog("Envoi des données à " + targetIP + " : r=" + String(incomingData->q_real) + " i=" + String(incomingData->q_i) + " j=" + String(incomingData->q_j) + " k=" + String(incomingData->q_k));
    clientCapteur.flush();
    clientCapteur.stop();
    return true;
}