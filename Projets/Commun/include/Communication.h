#pragma once

#include <Arduino.h>
#include <WiFi.h>

// Configuration pour le mode extérieur (Point d'Accès)
#define ap_ssid "Telescope_Net"
#define ap_password "12345678" // 8 caractères min.

#define portTCP 8080

// const char* ap_ssid       = "Telescope_Net";
// const char* ap_password   = "12345678"; // 8 caractères min.

typedef struct IMUData
{
    // char messageText[32];
    uint8_t sensorId;
    float accuracy;
    uint8_t precision;

    float q_real;
    float q_i;
    float q_j;
    float q_k;

    uint64_t imu_timestamp; // Timestamp du capteur en microsecondes
    uint64_t esp_timestamp; // Timestamp de l'ESP en millisecondes

    bool sent = true; // Indique si le message a été envoyé avec succès
    bool treated = false;
} IMUData;

// // ⚠️ METS ICI L'ADRESSE MAC DE TON ESP32-S3 PRINCIPAL
// uint8_t s3Address228[] = {0x98, 0x3D, 0xAE, 0x61, 0x9B, 0x64};
// uint8_t s3Address227[] = {0x30, 0xED, 0xA0, 0xBD, 0x56, 0xDC}; // Adresse MAC de l'ESP32-S3 principal
