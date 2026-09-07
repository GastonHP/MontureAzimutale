#include <Arduino.h>
#include <WiFi.h>
// #include <esp_now.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "Config.hpp"
#include "OTA.hpp"
#include "log.hpp"
#include "MonServeur.hpp"
#include "Communication.hpp"

#define BNO_ADDR 0x4B
#define BNO_INT_PIN 19

Adafruit_BNO08x bno;
sh2_SensorValue_t sensorValue;
volatile bool bnoDataReady = false;

#define MAX_CAPTEURS_DATA 4
const uint8_t NO_SENSOR_ID = -1;
const uint8_t STILL_ALIVE = -2;

CapteursMessage data[MAX_CAPTEURS_DATA];

CapteursMessage CAPTEURSData;

// esp_now_peer_info_t peerInfo;

const float SEUIL_MOUVEMENT = 0.0005;

void IRAM_ATTR bnoInterrupt() { bnoDataReady = true; }

CapteursMessage *getMessage(uint8_t sensorID)
{
    // Serial.println("getMessage sensorId=" + String(sensorID, DEC));
    for (int i = 0; i < MAX_CAPTEURS_DATA; i++)
        if (data[i].sensorId == sensorID)
            return &data[i];
    for (int i = 0; i < MAX_CAPTEURS_DATA; i++)
        if (data[i].sensorId == NO_SENSOR_ID)
        {
            data[i].sensorId = sensorID;
            return &data[i];
        }
    return nullptr;
}

bool addMessage(CapteursMessage *m)
{
    CapteursMessage *dest = getMessage(m->sensorId);
    if (dest == nullptr)
        dest = getMessage(NO_SENSOR_ID);
    if (dest == nullptr)
        return false;
    memcpy(dest, m, sizeof(CapteursMessage));
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

void setup()
{
    Serial.setTxBufferSize(2048);
    Serial.begin(115200);

    delay(1000);
    for (int i = 0; i < MAX_CAPTEURS_DATA; i++)
        data[i].sensorId = NO_SENSOR_ID;

    Config::setup();
    logGen("Config::setup() done.", false);
#ifdef OTA_ACTIF
    OTA::setup();
#endif

    Communication::setup(Config::NetworkHP());
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

    attachInterrupt(digitalPinToInterrupt(BNO_INT_PIN), bnoInterrupt, FALLING);

    // ✍️ On pré-remplit le texte fixe une bonne fois pour toutes au démarrage
    // strncpy évite les dépassements de mémoire si le texte est trop long
    // strncpy(CAPTEURSData_SH2_ROTATION_VECTOR.messageText, "ROTATION_VECTOR", sizeof(CAPTEURSData_SH2_ROTATION_VECTOR.messageText));
    // strncpy(CAPTEURSData_SH2_ARVR_STABILIZED_RV.messageText, "ARVR_STABILIZED_RV", sizeof(CAPTEURSData_SH2_ROTATION_VECTOR.messageText));
    // strncpy(CAPTEURSData_SH2_GAME_ROTATION_VECTOR.messageText, "GAME_ROTATION_VECTOR", sizeof(CAPTEURSData_SH2_ROTATION_VECTOR.messageText));

    MonServeur::setup();
}

unsigned long lastSendTime = 0;
unsigned long lastStillAlive = 0;

int sendCAPTEURSData()
{
    Serial.println("sendCAPTEURSData");
    int n = 0;
    if(!Communication::openConnection())
    {
        Serial.println("Communication::openConnection() FAILED");
        return 0;
    }
    for (int i = 0; i < MAX_CAPTEURS_DATA; i++)
    {
        CapteursMessage *m = &data[i];
        Serial.println(String(i, DEC) + ":" + String(m->sensorId, DEC) + ":" + m->sent);
        if (!m->sent && m->sensorId >= 0)
        {
            Communication::send(m);
            Serial.printf("%d: Type: %d r=%f i=%f j=%f k=%f\n", millis(), m->sensorId, m->q_real, m->q_i, m->q_j, m->q_k);
            m->sent = true;
            n++;
        }
    }
    Communication::closeConnection();
    return n;
}

String getJson()
{
    String json = "{";
    json += "\"ssid\": \"" + WiFi.SSID() + "\",";
    json += "\"channel\": " + String(WiFi.channel(), DEC) + ",";
    json += "\"test\": { \"texte\": \"zsxedcrfv\" },";
    for (int i = 0; i < MAX_CAPTEURS_DATA; i++)
    {
        CapteursMessage *m = &data[i];
        String prefix = "";
        switch (m->sensorId)
        {
        case SH2_ARVR_STABILIZED_RV:
            prefix = "arvr_";
            break;
        case SH2_ROTATION_VECTOR:
            prefix = "rv_";
            break;
        case SH2_GAME_ROTATION_VECTOR:
            prefix = "grv_";
            break;
        }
        if (prefix.length() > 0)
        {
            json += "\"" + prefix + "r\":" + String(m->q_real) + ",";
            json += "\"" + prefix + "i\":" + String(m->q_i) + ",";
            json += "\"" + prefix + "j\":" + String(m->q_j) + ",";
            json += "\"" + prefix + "k\":" + String(m->q_k) + ",";
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
#endif
    // delay(10);
    #define TempsEntreEnvoiData 1000
    if (millis() - lastSendTime >= TempsEntreEnvoiData) // Envoi toutes les 50 ms
    {
        lastSendTime = millis();
        // Serial.println();
        int n = sendCAPTEURSData();
        if (n > 0)
            Log::addLog("sendCAPTEURSData():" + String(n, DEC));
        MonServeur::send(getJson());
    }
    if (millis() - lastStillAlive >= 10000) // Envoi toutes les 10 secondes d'un message "still alive" pour indiquer que le capteur est toujours actif
    {
        lastStillAlive = millis();
        CapteursMessage *m = getMessage(STILL_ALIVE);
        if (m != nullptr)
        {
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
            CapteursMessage *m = getMessage(sensorValue.sensorId);
            if (m != nullptr)
            {
                // Serial.print("@");
                m->sensorId = sensorValue.sensorId;
                float current_real;
                float current_i;
                float current_j;
                float current_k;

                if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
                {
                    current_real = sensorValue.un.rotationVector.real;
                    current_i = sensorValue.un.rotationVector.i;
                    current_j = sensorValue.un.rotationVector.j;
                    current_k = sensorValue.un.rotationVector.k;

                    // Vérification du filtre de mouvement
                    if (abs(current_real - m->q_real) > SEUIL_MOUVEMENT ||
                        abs(current_i - m->q_i) > SEUIL_MOUVEMENT)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)
                        m->accuracy = sensorValue.un.rotationVector.accuracy * 57.2958; // Convertir en degrés
                        m->precision = sensorValue.status & 0x03;                       // Précision du capteur (0-3)
                        m->imu_timestamp = sensorValue.timestamp;                       // Timestamp du BNO085
                        m->cap_timestamp = millis();                                    // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des Quaternions
                        m->q_real = current_real;
                        m->q_i = current_i;
                        m->q_j = current_j;
                        m->q_k = current_k;
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
                    if (abs(current_real - m->q_real) > SEUIL_MOUVEMENT ||
                        abs(current_i - m->q_i) > SEUIL_MOUVEMENT)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)

                        m->accuracy = sensorValue.un.arvrStabilizedRV.accuracy * 57.2958; // Convertir en degrés
                        m->precision = sensorValue.status & 0x03;                         // Précision du capteur (0-3)
                        m->imu_timestamp = sensorValue.timestamp;                         // Timestamp du BNO085
                        m->cap_timestamp = millis();                                      // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des Quaternions
                        m->q_real = current_real;
                        m->q_i = current_i;
                        m->q_j = current_j;
                        m->q_k = current_k;
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
                    if (abs(current_real - m->q_real) > SEUIL_MOUVEMENT ||
                        abs(current_i - m->q_i) > SEUIL_MOUVEMENT)
                    {
                        // 1. Remplissage du type de données (ex: 101 pour Rotation Vector)

                        m->accuracy = -1;
                        m->precision = sensorValue.status & 0x03; // Précision du capteur (0-3)
                        m->imu_timestamp = sensorValue.timestamp; // Timestamp du BNO085
                        m->cap_timestamp = millis();              // Timestamp de l'ESP32 au moment de l'envoi

                        // 2. Remplissage des Quaternions
                        m->q_real = current_real;
                        m->q_i = current_i;
                        m->q_j = current_j;
                        m->q_k = current_k;
                        addMessage(m);
                    }
                }
            }
        }
    }
}
