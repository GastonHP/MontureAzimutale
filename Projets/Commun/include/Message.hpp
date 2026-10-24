#pragma once
#include "Arduino.h"
// #include <Adafruit_BNO08x.h>
#include "../../Commun/include/Communication.h"

typedef enum MessageType
{
    NO_MESSAGE_TYPE = SH2_MAX_SENSOR_ID + 0x10,
    GPS_DATA = SH2_MAX_SENSOR_ID + 0x11,
    STILL_ALIVE = SH2_MAX_SENSOR_ID + 0x12,
    STRING_256 = SH2_MAX_SENSOR_ID + 0x13,
    ACK = SH2_MAX_SENSOR_ID + 0x14,
    SEARCH_CLIENT = SH2_MAX_SENSOR_ID + 0x15,

    IMU_ROTATION_VECTOR = SH2_ROTATION_VECTOR,
    IMU_ARVR_STABILIZED_RV = SH2_ARVR_STABILIZED_RV,
    IMU_GAME_ROTATION_VECTOR = SH2_GAME_ROTATION_VECTOR,
    IMU_GRAVITY = SH2_GRAVITY

} MessageType;

typedef enum MessageStatus
{
    UNKNOWN = 0,
    ToSend = 1,
    Sent = 2,
    Received = 3,
    Treated = 4
} MessageStatus;

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

typedef struct ACKInfo
{
    uint32_t IPaddress;
} ACKInfo;

typedef struct SEARCH_CLIENTInfo
{
    // IPAddress(uint32_t address);
    uint32_t IPaddress;
} SEARCH_CLIENTInfo;

typedef struct MessageData
{
    char header[32]; // "IMU_DATA", "GPS_DATA", "STILL_ALIVE", "STRING_256"
    uint16_t version = protocolVersion;
    uint32_t number;
    MessageType messageType;
    MessageStatus status;
    uint64_t mon_timestamp; // timestamp du module commande
    union
    {
        GAME_ROTATION_VECTOR_Info gameRotationVectorInfo;
        ARVR_STABILIZED_RV_Info arvrStabilizedRVInfo;
        ROTATION_VECTOR_Info rotationVectorInfo;
        GPSInfo gpsInfo;
        StringInfo stringInfo;
        GRAVITY_Info gravityInfo;
        ACKInfo ackInfo;
        SEARCH_CLIENTInfo search_clientInfo;
    };
} MessageData;

#define MAX_MESSAGE_DATA 20

class Message
{
public:
    static void setup();
    static void loop();
    static MessageData *getSendMessage(MessageType messageType);
    static MessageData *getReceiveMessage(MessageType messageType);
    static MessageData *getMessageToSend();
    static MessageData *getMessageToTreat();
    static MessageData *getMessageByIndex(int index);
    static bool addMessageToSend(MessageData *m);
    static bool addReceivedMessage(MessageData *m);

private:
    static MessageData data[MAX_MESSAGE_DATA];
};