#include "imu.hpp"
#include "config.hpp"
#include "Log.hpp"
#include "Communication.hpp"
#include "EulerAngles.hpp"
#include <Adafruit_BNO08x.h>

IMUData Imu::lastdata;
Capteur Imu::rotationCapteur = Capteur(Capteur::TYPE::ROTATION);
Capteur Imu::ARVRCapteur = Capteur(Capteur::TYPE::ARVR);
Capteur Imu::gameRotationCapteur = Capteur(Capteur::TYPE::GAME_ROTATION);

void Imu::setup()
{
    Communication::setup(Config::NetworkHP());
}

bool Imu::saveIMUData(IMUData *m)
{
    memcpy(&lastdata, m, sizeof(IMUData));
    lastdata.treated = false;
    switch (m->sensorId)
    {
    case Capteur::TYPE::ARVR:
        return ARVRCapteur.addImuData(m);
        break;
    case Capteur::TYPE::ROTATION:
        return rotationCapteur.addImuData(m);
        break;
    case Capteur::TYPE::GAME_ROTATION:
        return gameRotationCapteur.addImuData(m);
        break;
    }
    return false;
}

IMUData *Imu::hasNewData()
{
    if (lastdata.treated == false)
    {
        // lastdata.treated=true;
        return &lastdata;
    }
    return nullptr;
}

String Imu::IMUDataToString(IMUData *data)
{
    String str = "IMUData: ";
    str += "q_real=" + String(data->q_real, 4) + ", ";
    str += "q_i=" + String(data->q_i, 4) + ", ";
    str += "q_j=" + String(data->q_j, 4) + ", ";
    str += "q_k=" + String(data->q_k, 4) + ", ";
    str += "imu_timestamp=" + String(data->imu_timestamp) + ", ";
    str += "cap_timestamp=" + String(data->cap_timestamp) + ", ";
    str += "mon_timestamp=" + String(data->mon_timestamp) + ", ";
    str += "sensorId=" + String(data->sensorId) + ", ";
    str += "accuracy=" + String(data->accuracy, 4) + ", ";
    str += "precision=" + String(data->precision) + ", ";
    str += "sent=" + String(data->sent ? "true" : "false") + ", ";
    str += "treated=" + String(data->treated ? "true" : "false");
    return str;
}
unsigned long lastLog = millis();
void Imu::loop()
{
    if (Communication::receive())
    {
        lastLog = millis();
        Log::addLog(Communication::error);
    }
    else
    {
        if (millis() - lastLog > 3000)
        {
            lastLog = millis();
            Log::addLog(Communication::error);
        }
    }
}
// #include <Arduino.h>
// #include <WiFi.h>
// #include "../../../Projets/Commun/include/Communication.h"
// #include "Log.hpp"

// // 📦 LA STRUCTURE (Doit être STRICTEMENT IDENTIQUE à celle de l'émetteur)
// // Instance locale pour stocker les données reçues
// IMUData incomingData;

// // 📥 FONCTION DE RAPPEL (Callback) : Exécutée automatiquement à la réception d'un paquet
// void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataRaw, int len)
// {
//     Log::addLog("OnDataRecv", true);
//     // 1. On copie les données brutes reçues dans notre structure propre
//     memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));

//     // 2. Affichage des informations dans le terminal de l'ESP32-S3
//     Serial.println("\n--- 📡 PAQUET ESP-NOW REÇU ---");

//     // Affichage de l'adresse MAC de l'émetteur
//     Serial.printf("Provenance (MAC) : %02X:%02X:%02X:%02X:%02X:%02X\n",
//                   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

//     // Affichage des données enrichies
//     Serial.printf("Nom du Satellite : %s\n", incomingData.messageText);
//     Serial.printf("Type de donnée   : %d\n", incomingData.sensorId);

//     // Affichage des Quaternions reçus
//     Serial.printf("Quaternions      : Real=%.4f | I=%.4f | J=%.4f | K=%.4f\n",
//                   incomingData.q_real, incomingData.q_i, incomingData.q_j, incomingData.q_k);

//     Serial.println("--------------------------------");
// }

// void imu_setup()
// {
//     Serial.println("\n=====================================");
//     Serial.println("      ESP32-S3 RECENTRALISATION      ");
//     Serial.println("=====================================");

//     // 1. Mettre le Wi-Fi en mode Station (Obligatoire pour ESP-NOW)
//     WiFi.mode(WIFI_AP_STA);

//     // 2. Afficher l'adresse MAC du S3
//     // (C'est cette adresse qu'il faut copier dans le code de l'émetteur !)
//     Serial.print("👉 ADRESSE MAC DE CE S3 : ");
//     Serial.println(WiFi.macAddress());

//     // 3. Initialiser ESP-NOW
//     if (esp_now_init() != ESP_OK)
//     {
//         Serial.println("❌ Erreur d'initialisation ESP-NOW");
//         Log::addLog("❌ Erreur d'initialisation ESP-NOW");
//         return;
//     }
//     Serial.println("✅ ESP-NOW initialisé avec succès.");
//     Log::addLog("✅ ESP-NOW initialisé avec succès.");

//     // 4. Enregistrer la fonction de rappel pour la réception
//     esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

//     Serial.println("📡 En écoute des signaux du télescope...");
// }

// void imu_loop()
// {
//     // Le loop reste totalement vide !
//     // C'est la fonction OnDataRecv() qui gère tout de manière asynchrone dès qu'un signal arrive.
// }