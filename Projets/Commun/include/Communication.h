#pragma once

#include <Arduino.h>
#include <WiFi.h>

// Configuration pour le mode extérieur (Point d'Accès)
#define ap_ssid "Commande_net"
#define ap_password "12345678" // 8 caractères min.

#define protocolVersion 3

typedef enum MessageType
{
    IMU_DATA = 100,
    STILL_ALIVE = 104,
    STRING_512 = 202
} MessageType;

typedef struct MessageInfo
{
    uint16_t version = protocolVersion;
    MessageType messageType;
    bool sent = true;       // Indique si le message a été envoyé avec succès
    bool treated = false;   // Indique si le message a été traité avec succès
    bool received = false;  // Indique si le message a été reçu avec succès
    uint64_t mon_timestamp; // timestamp du module commande
} MessageInfo;

typedef struct CapteursInfo
{
    // partie IMU
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
} CapteursInfo;

typedef struct CapteursMessage
{
    MessageInfo messageInfo;
    CapteursInfo capteursInfo;
} CapteurMessage;

typedef struct StringMessage
{
    MessageInfo messageInfo;
    char messageText[512];
} StringMessage;