#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_BNO08x.h>

// Configuration pour le mode extérieur (Point d'Accès)
#define ap_ssid "Commande_net"
#define ap_password "12345678" // 8 caractères min.

#define protocolVersion 4
#define udpLocalPort 4210 // Port d'écoute local

typedef enum MessageType
{
    NO_MESSAGE_TYPE = 0xff,
    GPS_DATA = 0x80,
    STILL_ALIVE = 0xc0,
    STRING_256 = 0xf0,
    IMU_ROTATION_VECTOR = SH2_ROTATION_VECTOR,
    IMU_ARVR_STABILIZED_RV = SH2_ARVR_STABILIZED_RV,
    IMU_GAME_ROTATION_VECTOR = SH2_GAME_ROTATION_VECTOR,
    IMU_GRAVITY = SH2_GRAVITY
} MessageType;

typedef struct ROTATION_VECTOR_Info
{
    uint64_t imu_timestamp; // Timestamp du capteur en microsecondes
    uint64_t cap_timestamp; // Timestamp de l'ESP capteur en millisecondes
    uint8_t precision;
    // partie quaternion
    float q_real;
    float q_i;
    float q_j;
    float q_k;
    // partie précision
    float accuracy;
} ROTATION_VECTOR_Info;

typedef struct ARVR_STABILIZED_RV_Info
{
    uint64_t imu_timestamp; // Timestamp du capteur en microsecondes
    uint64_t cap_timestamp; // Timestamp de l'ESP capteur en millisecondes
    uint8_t precision;
    // partie quaternion
    float q_real;
    float q_i;
    float q_j;
    float q_k;
    // partie précision
    float accuracy;
} ARVR_STABILIZED_RV_Info;

typedef struct GAME_ROTATION_VECTOR_Info
{
    uint64_t imu_timestamp; // Timestamp du capteur en microsecondes
    uint64_t cap_timestamp; // Timestamp de l'ESP capteur en millisecondes
    uint8_t precision;
    // partie quaternion
    float q_real;
    float q_i;
    float q_j;
    float q_k;
} GAME_ROTATION_VECTOR_Info;

typedef struct GRAVITY_Info
{
    uint64_t imu_timestamp; // Timestamp du capteur en microsecondes
    uint64_t cap_timestamp; // Timestamp de l'ESP capteur en millisecondes
    float x;
    float y;
    float z;
} GRAVITY_Info;

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
    char header[32]; // "IMU_DATA", "GPS_DATA", "STILL_ALIVE", "STRING_256"
    uint16_t version = protocolVersion;
    uint32_t number;
    MessageType messageType;
    bool sent = true;       // Indique si le message a été envoyé avec succès
    bool treated = false;   // Indique si le message a été traité avec succès
    bool received = false;  // Indique si le message a été reçu avec succès
    uint64_t mon_timestamp; // timestamp du module commande
    union
    {
        GAME_ROTATION_VECTOR_Info gameRotationVectorInfo;
        ARVR_STABILIZED_RV_Info arvrStabilizedRVInfo;
        ROTATION_VECTOR_Info rotationVectorInfo;
        GPSInfo gpsInfo;
        StringInfo stringInfo;
        GRAVITY_Info gravityInfo;
    };
} MessageData;