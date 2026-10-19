#pragma once

#include <Arduino.h>
#include <WiFi.h>

// Configuration pour le mode extérieur (Point d'Accès)
#define ap_ssid "Commande_net"
#define ap_password "12345678" // 8 caractères min.

#define protocolVersion 4
#define udpLocalPort 4210 // Port d'écoute local

typedef enum MessageType
{
    IMU_DATA = 100,
    GPS_DATA = 101,
    STILL_ALIVE = 104,
    STRING_256 = 202
} MessageType;

typedef struct IMUInfo
{
    uint8_t sensorId;
    // partie quaternion
    float q_real;
    float q_i;
    float q_j;
    float q_k;
    // partie précision
    float accuracy;
    uint8_t precision;
    // partie timestamp
    uint64_t imu_timestamp; // Timestamp du capteur en microsecondes
    uint64_t cap_timestamp; // Timestamp de l'ESP capteur en millisecondes
} IMUInfo;

typedef struct GPSInfo
{
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float course;
    uint8_t satellites;
    uint8_t fixType;
    uint64_t gps_timestamp; // Timestamp du GPS en microsecondes
    uint64_t cap_timestamp; // Timestamp de l'ESP capteur en millisecondes
} GPSInfo;

typedef struct StringInfo
{
    uint16_t stringLength;
    char messageText[256];
} StringInfo;

typedef struct MessageData
{
    uint16_t version = protocolVersion;
    uint32_t number;
    MessageType messageType;
    bool sent = true;       // Indique si le message a été envoyé avec succès
    bool treated = false;   // Indique si le message a été traité avec succès
    bool received = false;  // Indique si le message a été reçu avec succès
    uint64_t mon_timestamp; // timestamp du module commande
    union
    {
        IMUInfo imuInfo;
        GPSInfo gpsInfo;
        StringInfo stringInfo;
    };
} MessageData;