#include "imu.hpp"
#include "config.hpp"
#include "Communication.hpp"
#include "EulerAngles.hpp"
#include <Adafruit_BNO08x.h>

IMUData Imu::data[MAX_IMU_DATA];

void Imu::setup()
{
    for (int i = 0; i < MAX_IMU_DATA; i++)
    {
        data[i].sensorId = NO_SENSOR_ID;
        data[i].treated = true;
    }
    Communication::setup(Config::NetworkHP());
}

IMUData *Imu::getSavedMessage(uint8_t sensorID)
{
    return getMessage(sensorID | 0x80);
}

bool Imu::saveMessage(uint8_t sensorId)
{
    IMUData *m = getMessage(sensorId);
    if (m != nullptr)
    {
        IMUData *s = getMessage(sensorId | 0x80);
        if (s != nullptr)
        {
            memcpy(s, m, sizeof(IMUData));
            return true;
        }
    }
    return false;
}
IMUData *Imu::getNewMessage()
{
    for (int i = 0; i < MAX_IMU_DATA; i++)
        if (data[i].treated == false && data[i].sensorId != NO_SENSOR_ID)
        {
            return &data[i];
        }
    return nullptr;
}

IMUData *Imu::getMessage(uint8_t sensorID)
{
    for (int i = 0; i < MAX_IMU_DATA; i++)
        if (data[i].sensorId == sensorID)
            return &data[i];
    for (int i = 0; i < MAX_IMU_DATA; i++)
        if (data[i].sensorId == NO_SENSOR_ID)
        {
            data[i].sensorId = sensorID;
            return &data[i];
        }
    return nullptr;
}

EulerAngles Imu::getEulerAngles(uint8_t sensorId)
{
    IMUData *m = getMessage(sensorId);
    if (m == nullptr)
        return EulerAngles(0, 0, 0);
    EulerAngles e = EulerAngles::getEulerFromQuaternion(m->q_i, m->q_j, m->q_k, m->q_real);
    // À ajouter dans votre fonction afficherInterface
    e.bno_timestamp = m->imu_timestamp;
    e.esp_timestamp = m->esp_timestamp;
    e.sensorId = m->sensorId;
    e.precision = m->precision;
    e.accuracy = m->accuracy;
    switch (sensorId)
    {
    case SH2_ARVR_STABILIZED_RV:
    case SH2_ROTATION_VECTOR:
    case SH2_GAME_ROTATION_VECTOR:
        e.valid = true;
        break;
    }
    return e;
}

bool Imu::addMessage(IMUData *m)
{
    IMUData *dest = getMessage(m->sensorId);
    if (dest == nullptr)
        return false;
    memcpy(dest, m, sizeof(IMUData));
    dest->treated = false;
    return true;
}

void Imu::loop()
{
    if (Communication::receive())
    {
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