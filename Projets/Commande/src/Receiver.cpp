#include "Receiver.hpp"
#include "capteur.hpp"
#include "Log.hpp"

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

String Receiver::error = "";
void Receiver::setup(bool networkHP = true)
{
    if (networkHP)
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

    quickEspNow.begin(1, 0, false);
    quickEspNow.onDataSent(dataSent);
    quickEspNow.onDataRcvd(dataReceived);
}