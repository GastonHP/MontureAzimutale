

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "../../../../MontureAzimutale/include/esp_now.hpp"

uint8_t s3Address[] = {0x34, 0x85, 0x18, 0x8F, 0x22, 0xB4};

#define BNO_ADDR 0x4B
#define BNO_INT_PIN 19

Adafruit_BNO08x bno;
sh2_SensorValue_t sensorValue;
volatile bool bnoDataReady = false;

struct_message imuData_SH2_ROTATION_VECTOR;
struct_message imuData_SH2_ARVR_STABILIZED_RV;
struct_message imuData_SH2_GAME_ROTATION_VECTOR;

esp_now_peer_info_t peerInfo;

const float SEUIL_MOUVEMENT = 0.0005;

void IRAM_ATTR bnoInterrupt() { bnoDataReady = true; }

void setup()
{
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
        return;

    memcpy(peerInfo.peer_addr, s3Address, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    pinMode(BNO_INT_PIN, INPUT_PULLUP);
    Wire.begin();

    if (!bno.begin_I2C(BNO_ADDR, &Wire))
    {
        while (1)
        {
            delay(10);
        }
    }
    #define interval_us 50000

    bno.enableReport(SH2_ARVR_STABILIZED_RV, interval_us);
    bno.enableReport(SH2_ROTATION_VECTOR, interval_us*3/2);
    bno.enableReport(SH2_GAME_ROTATION_VECTOR, interval_us*7/3);

    attachInterrupt(digitalPinToInterrupt(BNO_INT_PIN), bnoInterrupt, FALLING);

    // ✍️ On pré-remplit le texte fixe une bonne fois pour toutes au démarrage
    // strncpy évite les dépassements de mémoire si le texte est trop long
    strncpy(imuData_SH2_ROTATION_VECTOR.messageText, "ROTATION_VECTOR", sizeof(imuData_SH2_ROTATION_VECTOR.messageText));
    strncpy(imuData_SH2_ARVR_STABILIZED_RV.messageText, "ARVR_STABILIZED_RV", sizeof(imuData_SH2_ROTATION_VECTOR.messageText));
    strncpy(imuData_SH2_GAME_ROTATION_VECTOR.messageText, "GAME_ROTATION_VECTOR", sizeof(imuData_SH2_ROTATION_VECTOR.messageText));


    Serial.println("🚀 Émetteur prêt avec structure enrichie !");
}

unsigned long lastSendTime = 0;

void testAndSend(struct_message *m)
{
    if (!m->sent)
    {
        // Envoi complet (la taille s'adapte automatiquement avec sizeof)
        esp_now_send(s3Address, (uint8_t *)&m, sizeof(m));
        Serial.printf("%d:Envoi réussi de [%s] | Type: %d r=%f i=%f j=%f k=%f\n", millis(), m->messageText, m->sensorId, m->q_real, m->q_i, m->q_j, m->q_k);
        m->sent = true;
    }
}
void sendIMUData()
{
    testAndSend(&imuData_SH2_ROTATION_VECTOR);
    testAndSend(&imuData_SH2_ARVR_STABILIZED_RV);
    testAndSend(&imuData_SH2_GAME_ROTATION_VECTOR);
}

void loop()
{
    //delay(10);                           // Petite pause pour éviter de saturer le processeur
    if (millis() - lastSendTime >= 1000) // Envoi toutes les 50 ms
    {
        lastSendTime = millis();
        sendIMUData();
    }
    if (bnoDataReady)
    {
        bnoDataReady = false;

        while (bno.getSensorEvent(&sensorValue))
        {
            // On vérifie le type reçu du BNO085
            // Serial.print("*");
            if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
            {
                struct_message *m = &imuData_SH2_ROTATION_VECTOR;
                float current_real = sensorValue.un.rotationVector.real;
                float current_i = sensorValue.un.rotationVector.i;
                float current_j = sensorValue.un.rotationVector.j;
                float current_k = sensorValue.un.rotationVector.k;

                // Vérification du filtre de mouvement
                if (abs(current_real - m->q_real) > SEUIL_MOUVEMENT ||
                    abs(current_i - m->q_i) > SEUIL_MOUVEMENT)
                {
                    m->sent = false; // On marque le message comme non envoyé avant l'envoi

                    // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)
                    m->sensorId = SH2_ROTATION_VECTOR;
                    m->accuracy = sensorValue.un.rotationVector.accuracy * 57.2958; // Convertir en degrés
                    m->precision = sensorValue.status & 0x03;                       // Précision du capteur (0-3)
                    m->imu_timestamp = sensorValue.timestamp;                       // Timestamp du BNO085
                    m->esp_timestamp = millis();                                    // Timestamp de l'ESP32 au moment de l'envoi

                    // 2. Remplissage des Quaternions
                    m->q_real = current_real;
                    m->q_i = current_i;
                    m->q_j = current_j;
                    m->q_k = current_k;
                }
            }
            if (sensorValue.sensorId == SH2_ARVR_STABILIZED_RV)
            {
                float current_real = sensorValue.un.arvrStabilizedRV.real;
                float current_i = sensorValue.un.arvrStabilizedRV.i;
                float current_j = sensorValue.un.arvrStabilizedRV.j;
                float current_k = sensorValue.un.arvrStabilizedRV.k;
                struct_message *m = &imuData_SH2_ARVR_STABILIZED_RV;
                // Vérification du filtre de mouvement
                if (abs(current_real - m->q_real) > SEUIL_MOUVEMENT ||
                    abs(current_i - m->q_i) > SEUIL_MOUVEMENT)
                {
                    m->sent = false; // On marque le message comme non envoyé avant l'envoi

                    // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)
                    m->sensorId = SH2_ARVR_STABILIZED_RV;
                    m->accuracy = sensorValue.un.arvrStabilizedRV.accuracy * 57.2958; // Convertir en degrés
                    m->precision = sensorValue.status & 0x03;                         // Précision du capteur (0-3)
                    m->imu_timestamp = sensorValue.timestamp;                         // Timestamp du BNO085
                    m->esp_timestamp = millis();                                      // Timestamp de l'ESP32 au moment de l'envoi

                    // 2. Remplissage des Quaternions
                    m->q_real = current_real;
                    m->q_i = current_i;
                    m->q_j = current_j;
                    m->q_k = current_k;
                }
            }
            if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR)
            {
                float current_real = sensorValue.un.gameRotationVector.real;
                float current_i = sensorValue.un.gameRotationVector.i;
                float current_j = sensorValue.un.gameRotationVector.j;
                float current_k = sensorValue.un.gameRotationVector.k;
                struct_message *m = &imuData_SH2_GAME_ROTATION_VECTOR;
                // Vérification du filtre de mouvement
                if (abs(current_real - m->q_real) > SEUIL_MOUVEMENT ||
                    abs(current_i - m->q_i) > SEUIL_MOUVEMENT)
                {
                    m->sent = false; // On marque le message comme non envoyé avant l'envoi

                    // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)
                    m->sensorId = SH2_GAME_ROTATION_VECTOR;
                    m->accuracy = -1;
                    m->precision = sensorValue.status & 0x03; // Précision du capteur (0-3)
                    m->imu_timestamp = sensorValue.timestamp; // Timestamp du BNO085
                    m->esp_timestamp = millis();              // Timestamp de l'ESP32 au moment de l'envoi

                    // 2. Remplissage des Quaternions
                    m->q_real = current_real;
                    m->q_i = current_i;
                    m->q_j = current_j;
                    m->q_k = current_k;
                }
            }
        }
        // Serial.println(" -END-");
    }
}
