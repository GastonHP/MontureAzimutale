#include <Arduino.h>
#include <WiFi.h>
// #include <esp_now.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "Config.hpp"
#include "OTA.hpp"
#include "log.hpp"
#include "MonServeur.hpp"
#include "Sender.hpp"

#define BNO_ADDR 0x4B
#define BNO_INT_PIN 19

Adafruit_BNO08x bno;
sh2_SensorValue_t sensorValue;
volatile bool bnoDataReady = false;

#define MAX_MESSAGES_DATA 10

MessageData data[MAX_MESSAGES_DATA];

static MessageData messageData;

// esp_now_peer_info_t peerInfo;

const float SEUIL_MOUVEMENT = 0.001;
const float SEUIL_GRAVITY = 0.01;

void IRAM_ATTR bnoInterrupt() { bnoDataReady = true; }

MessageData *getMessage(MessageType messageType)
{
    // Serial.println("getMessage messageType=" + String(messageType, DEC));
    for (int i = 0; i < MAX_MESSAGES_DATA; i++)
        if (data[i].messageType == messageType)
            return &data[i];
    for (int i = 0; i < MAX_MESSAGES_DATA; i++)
        if (data[i].messageType == NO_MESSAGE_TYPE)
        {
            data[i].messageType = messageType;
            return &data[i];
        }
    return nullptr;
}

bool addMessage(MessageData *m)
{
    MessageData *dest = getMessage(m->messageType);
    if (dest == nullptr)
        dest = getMessage(NO_MESSAGE_TYPE);
    if (dest == nullptr)
        return false;
    memcpy(dest, m, sizeof(MessageData));
    dest->treated = false;
    dest->sent = false;
    return true;
}

static void logGen(String s, bool LogFile = true)
{
    Serial.println("==>" + s);
    if (LogFile == true)
        Log::addLog(s);
}

static MessageData stillAliveMessage;

void setup()
{
    Serial.setTxBufferSize(2048);
    Serial.begin(115200);

    delay(1000);
    for (int i = 0; i < MAX_MESSAGES_DATA; i++)
        data[i].messageType = NO_MESSAGE_TYPE;

    Config::setup();
    logGen("Config::setup() done.", false);
    generic_setup();
#ifdef OTA_ACTIF
    OTA::setup();
#endif

    Sender::setup(Config::NetworkHP());
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
    bno.enableReport(SH2_ROTATION_VECTOR, interval_us * 3 / 2);
    bno.enableReport(SH2_GAME_ROTATION_VECTOR, interval_us * 7 / 3);
    bno.enableReport(SH2_GRAVITY, interval_us * 3 / 2);

    attachInterrupt(digitalPinToInterrupt(BNO_INT_PIN), bnoInterrupt, FALLING);

    // ✍️ On pré-remplit le texte fixe une bonne fois pour toutes au démarrage
    // strncpy évite les dépassements de mémoire si le texte est trop long
    // strncpy(messageData_SH2_ROTATION_VECTOR.messageText, "ROTATION_VECTOR", sizeof(messageData_SH2_ROTATION_VECTOR.messageText));
    // strncpy(messageData_SH2_ARVR_STABILIZED_RV.messageText, "ARVR_STABILIZED_RV", sizeof(messageData_SH2_ROTATION_VECTOR.messageText));
    // strncpy(messageData_SH2_GAME_ROTATION_VECTOR.messageText, "GAME_ROTATION_VECTOR", sizeof(messageData_SH2_ROTATION_VECTOR.messageText));

    MonServeur::setup();
}

unsigned long lastSendTime = 0;
unsigned long lastStillAlive = 0;

int sendmessageData()
{
    Serial.println("sendmessageData");
    int n = 0;
    if (!Sender::openConnection())
    {
        Serial.println("Sender::openConnection() FAILED");
        return 0;
    }
    for (int i = 0; i < MAX_MESSAGES_DATA; i++)
    {
        MessageData *m = &data[i];
        Serial.println(String(i, DEC) + ":" + String(m->messageType, DEC) + ":" + m->sent);
        if (!m->sent && m->messageType >= 0)
        {
            Sender::send(m);
            // Serial.printf("%d: Type: %d r=%f i=%f j=%f k=%f\n", millis(), m->messageType, m->imuInfo.q_real, m->imuInfo.q_i, m->imuInfo.q_j, m->imuInfo.q_k);
            m->sent = true;
            n++;
        }
    }
    Sender::closeConnection();
    return n;
}

String getJson()
{
    String json = "{";
    json += "\"ssid\": \"" + WiFi.SSID() + "\",";
    json += "\"channel\": " + String(WiFi.channel(), DEC) + ",";
    json += "\"test\": { \"texte\": \"zsxedcrfv\" },";
    for (int i = 0; i < MAX_MESSAGES_DATA; i++)
    {
        MessageData *m = &data[i];
        switch (m->messageType)
        {
        case IMU_ARVR_STABILIZED_RV:
            json += "\"arvr_r\":" + String(m->arvrStabilizedRVInfo.q_real) + ",";
            json += "\"arvr_i\":" + String(m->arvrStabilizedRVInfo.q_i) + ",";
            json += "\"arvr_j\":" + String(m->arvrStabilizedRVInfo.q_j) + ",";
            json += "\"arvr_k\":" + String(m->arvrStabilizedRVInfo.q_k) + ",";
            json += "\"arvr_accuracy\":" + String(m->arvrStabilizedRVInfo.accuracy) + ",";
            json += "\"arvr_precision\":" + String(m->arvrStabilizedRVInfo.precision) + ",";
            break;
        case IMU_ROTATION_VECTOR:
            json += "\"rv_r\":" + String(m->rotationVectorInfo.q_real) + ",";
            json += "\"rv_i\":" + String(m->rotationVectorInfo.q_i) + ",";
            json += "\"rv_j\":" + String(m->rotationVectorInfo.q_j) + ",";
            json += "\"rv_k\":" + String(m->rotationVectorInfo.q_k) + ",";
            json += "\"rv_accuracy\":" + String(m->rotationVectorInfo.accuracy) + ",";
            json += "\"rv_precision\":" + String(m->rotationVectorInfo.precision) + ",";
            break;
        case IMU_GAME_ROTATION_VECTOR:
            json += "\"rv_r\":" + String(m->gameRotationVectorInfo.q_real) + ",";
            json += "\"rv_i\":" + String(m->gameRotationVectorInfo.q_i) + ",";
            json += "\"rv_j\":" + String(m->gameRotationVectorInfo.q_j) + ",";
            json += "\"rv_k\":" + String(m->gameRotationVectorInfo.q_k) + ",";
            json += "\"rv_precision\":" + String(m->gameRotationVectorInfo.precision) + ",";
            break;
        case IMU_GRAVITY:
            json += "\"gravity_x\":" + String(m->gravityInfo.x) + ",";
            json += "\"gravity_y\":" + String(m->gravityInfo.y) + ",";
            json += "\"gravity_z\":" + String(m->gravityInfo.z) + ",";
            break;
        }
    }
    // json += "\"arvr_yaw\":" + String(ARVR_STABILIZED_RV_anglesActuels.yaw, 2) + ",";
    // json += "\"arvr_pitch\":" + String(ARVR_STABILIZED_RV_anglesActuels.pitch, 2) + ",";
    // json += "\"arvr_roll\":" + String(ARVR_STABILIZED_RV_anglesActuels.roll, 2) + ",";
    // json += "\"arvr_accuracy\":" + String(ARVR_STABILIZED_RV_anglesActuels.accuracy, 2) + ",";
    // json += "\"arvr_precision\":" + String(ARVR_STABILIZED_RV_anglesActuels.precision) + ",";
    // json += "\"arvr_timestamp\":\"" + bnoTimestampToString(ARVR_STABILIZED_RV_anglesActuels.bno_timestamp, ARVR_STABILIZED_RV_anglesActuels.esp_timestamp) + "\",";
    // json += "\"arvr_deltaALT\":\"" + Telescope::ARVR_STABILIZED_RV_calibration.deltaALT().toString() + "\",";
    // json += "\"arvr_deltaAZ\":\"" + Telescope::ARVR_STABILIZED_RV_calibration.deltaAZ().toString() + "\",";

    // json += "\"rotation_vector_yaw\":" + String(ROTATION_VECTOR_anglesActuels.yaw, 2) + ",";
    // json += "\"rotation_vector_pitch\":" + String(ROTATION_VECTOR_anglesActuels.pitch, 2) + ",";
    // json += "\"rotation_vector_roll\":" + String(ROTATION_VECTOR_anglesActuels.roll, 2) + ",";
    // json += "\"rotation_vector_accuracy\":" + String(ROTATION_VECTOR_anglesActuels.accuracy, 2) + ",";
    // json += "\"rotation_vector_precision\":" + String(ROTATION_VECTOR_anglesActuels.precision) + ",";
    // json += "\"rotation_vector_timestamp\":\"" + bnoTimestampToString(ROTATION_VECTOR_anglesActuels.bno_timestamp, ROTATION_VECTOR_anglesActuels.esp_timestamp) + "\",";
    // json += "\"rotation_vector_deltaALT\":\"" + Telescope::ROTATION_VECTOR_calibration.deltaALT().toString() + "\",";
    // json += "\"rotation_vector_deltaAZ\":\"" + Telescope::ROTATION_VECTOR_calibration.deltaAZ().toString() + "\",";

    // json += "\"game_rotation_vector_yaw\":" + String(GAME_ROTATION_VECTOR_anglesActuels.yaw, 2) + ",";
    // json += "\"game_rotation_vector_pitch\":" + String(GAME_ROTATION_VECTOR_anglesActuels.pitch, 2) + ",";
    // json += "\"game_rotation_vector_roll\":" + String(GAME_ROTATION_VECTOR_anglesActuels.roll, 2) + ",";
    // json += "\"game_rotation_vector_accuracy\":" + String(GAME_ROTATION_VECTOR_anglesActuels.accuracy, 2) + ",";
    // json += "\"game_rotation_vector_precision\":" + String(GAME_ROTATION_VECTOR_anglesActuels.precision) + ",";
    // json += "\"game_rotation_vector_timestamp\":\"" + bnoTimestampToString(GAME_ROTATION_VECTOR_anglesActuels.bno_timestamp, GAME_ROTATION_VECTOR_anglesActuels.esp_timestamp) + "\",";
    // json += "\"game_rotation_vector_deltaALT\":\"" + Telescope::GAME_ROTATION_VECTOR_calibration.deltaALT().toString() + "\",";
    // json += "\"game_rotation_vector_deltaAZ\":\"" + Telescope::GAME_ROTATION_VECTOR_calibration.deltaAZ().toString() + "\",";

    // json += "\"batt\":\"" + String(Batterie::lireTension(), 1) + "\",";
    // json += "\"sats\":\"" + String(GPSManager::satellites()) + "\",";

    // String logConcatene = "";
    // for (int i = Telescope::maxLogLines - 1; i >= 0; i--)
    // {
    //     logConcatene += Telescope::logBuffer[i] + "<br>";
    // }
    // json += "\"log\":\"" + logConcatene + "\",";
    json += "\"EOJ\":\"---\"}";
    return json;
}

void loop()
{
#ifdef OTA_ACTIF
    OTA::loop();
    if (OTA::started())
    {
        return;
    }
#endif
    generic_loop();

    Sender::loop();

// delay(10);
#define TempsEntreEnvoiData 1000
    if (millis() - lastSendTime >= TempsEntreEnvoiData) // Envoi toutes les 50 ms
    {
        lastSendTime = millis();
        // Serial.println();
        int n = sendmessageData();
        if (n > 0)
            Log::addLog("sendmessageData():" + String(n, DEC));
        MonServeur::send(getJson());
    }
    if (millis() - lastStillAlive >= 10000) // Envoi toutes les 10 secondes d'un message "still alive" pour indiquer que le capteur est toujours actif
    {
        MessageData *m = getMessage(MessageType::STILL_ALIVE);
        if (m != nullptr)
        {
            m->messageType = MessageType::STILL_ALIVE;
            m->mon_timestamp = millis();
            m->sent = false;              // Marquer le message comme non envoyé avant l'envoi
            m->treated = false;           // Marquer le message comme non traité avant l'envoi
            m->received = false;          // Marquer le message comme non reçu avant l'envoi
            m->version = protocolVersion; // Assurez-vous que la version est correctement définie
            lastStillAlive = millis();
            String stillAliveText = "En Vie! " + String(m->number, DEC) + " - " + String(m->mon_timestamp, DEC);
            strncpy(m->stringInfo.messageText, stillAliveText.c_str(), sizeof(m->stringInfo.messageText));
            m->stringInfo.stringLength = strlen(m->stringInfo.messageText);
            addMessage(m);
        }
    }

    if (bnoDataReady)
    {
        bnoDataReady = false;

        while (bno.getSensorEvent(&sensorValue))
        {
            // Log::addLog("bno getEvent", true);
            //  On vérifie le type reçu du BNO085
            //  Serial.print("*" + String(sensorValue.sensorId, DEC));
            MessageData *m = getMessage((MessageType)sensorValue.sensorId);
            if (m != nullptr)
            {
                // Serial.print("@");
                m->messageType = (MessageType)sensorValue.sensorId;
                float current_real;
                float current_i;
                float current_j;
                float current_k;
                float current_x;
                float current_y;
                float current_z;

                if (sensorValue.sensorId == SH2_GRAVITY)
                {
                    current_x = sensorValue.un.gravity.x;
                    current_y = sensorValue.un.gravity.y;
                    current_z = sensorValue.un.gravity.z;
                    // Vérification du filtre de mouvement
                    if (abs(current_x - m->gravityInfo.x) > SEUIL_GRAVITY ||
                        abs(current_y - m->gravityInfo.y) > SEUIL_GRAVITY ||
                        abs(current_z - m->gravityInfo.z) > SEUIL_GRAVITY)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)
                        m->gravityInfo.imu_timestamp = sensorValue.timestamp; // Timestamp du BNO085
                        m->gravityInfo.cap_timestamp = millis();              // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des coordonnées
                        m->gravityInfo.x = current_x;
                        m->gravityInfo.y = current_y;
                        m->gravityInfo.z = current_z;
                        addMessage(m);
                    }
                }
                if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
                {
                    current_real = sensorValue.un.rotationVector.real;
                    current_i = sensorValue.un.rotationVector.i;
                    current_j = sensorValue.un.rotationVector.j;
                    current_k = sensorValue.un.rotationVector.k;

                    // Vérification du filtre de mouvement
                    if (abs(current_real - m->rotationVectorInfo.q_real) > SEUIL_MOUVEMENT ||
                        abs(current_i - m->rotationVectorInfo.q_i) > SEUIL_MOUVEMENT)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)
                        m->rotationVectorInfo.accuracy = sensorValue.un.rotationVector.accuracy * 57.2958; // Convertir en degrés
                        m->rotationVectorInfo.precision = sensorValue.status & 0x03;                       // Précision du capteur (0-3)
                        m->rotationVectorInfo.imu_timestamp = sensorValue.timestamp;                       // Timestamp du BNO085
                        m->rotationVectorInfo.cap_timestamp = millis();                                    // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des Quaternions
                        m->rotationVectorInfo.q_real = current_real;
                        m->rotationVectorInfo.q_i = current_i;
                        m->rotationVectorInfo.q_j = current_j;
                        m->rotationVectorInfo.q_k = current_k;
                        addMessage(m);
                    }
                }
                if (sensorValue.sensorId == SH2_ARVR_STABILIZED_RV)
                {
                    current_real = sensorValue.un.arvrStabilizedRV.real;
                    current_i = sensorValue.un.arvrStabilizedRV.i;
                    current_j = sensorValue.un.arvrStabilizedRV.j;
                    current_k = sensorValue.un.arvrStabilizedRV.k;
                    // Vérification du filtre de mouvement
                    if (abs(current_real - m->arvrStabilizedRVInfo.q_real) > SEUIL_MOUVEMENT ||
                        abs(current_i - m->arvrStabilizedRVInfo.q_i) > SEUIL_MOUVEMENT)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)

                        m->arvrStabilizedRVInfo.accuracy = sensorValue.un.arvrStabilizedRV.accuracy * 57.2958; // Convertir en degrés
                        m->arvrStabilizedRVInfo.precision = sensorValue.status & 0x03;                         // Précision du capteur (0-3)
                        m->arvrStabilizedRVInfo.imu_timestamp = sensorValue.timestamp;                         // Timestamp du BNO085
                        m->arvrStabilizedRVInfo.cap_timestamp = millis();                                      // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des Quaternions
                        m->arvrStabilizedRVInfo.q_real = current_real;
                        m->arvrStabilizedRVInfo.q_i = current_i;
                        m->arvrStabilizedRVInfo.q_j = current_j;
                        m->arvrStabilizedRVInfo.q_k = current_k;
                        addMessage(m);
                    }
                }
                if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR)
                {
                    current_real = sensorValue.un.gameRotationVector.real;
                    current_i = sensorValue.un.gameRotationVector.i;
                    current_j = sensorValue.un.gameRotationVector.j;
                    current_k = sensorValue.un.gameRotationVector.k;

                    // Vérification du filtre de mouvement
                    if (abs(current_real - m->gameRotationVectorInfo.q_real) > SEUIL_MOUVEMENT ||
                        abs(current_i - m->gameRotationVectorInfo.q_i) > SEUIL_MOUVEMENT)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)

                        m->gameRotationVectorInfo.precision = sensorValue.status & 0x03; // Précision du capteur (0-3)
                        m->gameRotationVectorInfo.imu_timestamp = sensorValue.timestamp; // Timestamp du BNO085
                        m->gameRotationVectorInfo.cap_timestamp = millis();              // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des Quaternions
                        m->gameRotationVectorInfo.q_real = current_real;
                        m->gameRotationVectorInfo.q_i = current_i;
                        m->gameRotationVectorInfo.q_j = current_j;
                        m->gameRotationVectorInfo.q_k = current_k;
                        addMessage(m);
                    }
                }
            }
        }
    }
}
