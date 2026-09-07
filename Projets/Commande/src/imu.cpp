#include "imu.hpp"
#include "config.hpp"
#include "Log.hpp"
#include "Communication.hpp"
#include "EulerAngles.hpp"
#include <Adafruit_BNO08x.h>

CapteursMessage Imu::lastdata;
Capteur Imu::rotationCapteur = Capteur(Capteur::TYPE::ROTATION);
Capteur Imu::ARVRCapteur = Capteur(Capteur::TYPE::ARVR);
Capteur Imu::gameRotationCapteur = Capteur(Capteur::TYPE::GAME_ROTATION);

void Imu::setup()
{
    Communication::setup(Config::NetworkHP());
}

bool Imu::saveIMUData(CapteursMessage *m)
{
    memcpy(&lastdata, m, sizeof(CapteursMessage));
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

CapteursMessage *Imu::hasNewData()
{
    if (lastdata.treated == false)
    {
        // lastdata.treated=true;
        return &lastdata;
    }
    return nullptr;
}

String Imu::IMUDataToString(CapteursMessage *data)
{
    String str = "capteursData: ";
    str += "version=" + String(data->version) + ", ";
    str += "sent=" + String(data->sent ? "true" : "false") + ", ";
    str += "treated=" + String(data->treated ? "true" : "false") + ", ";
    str += "sensorId=" + String(data->sensorId) + ", ";
    str += "q_real=" + String(data->q_real, 4) + ", ";
    str += "q_i=" + String(data->q_i, 4) + ", ";
    str += "q_j=" + String(data->q_j, 4) + ", ";
    str += "q_k=" + String(data->q_k, 4) + ", ";

    str += "accuracy=" + String(data->accuracy, 4) + ", ";
    str += "precision=" + String(data->precision) + ", ";
    str += "imu_timestamp=" + String(data->imu_timestamp) + ", ";
    str += "cap_timestamp=" + String(data->cap_timestamp) + ", ";
    str += "mon_timestamp=" + String(data->mon_timestamp);
    return str;
}
unsigned long lastLog = millis();
void Imu::loop() { Communication::receive(); }

// // 📦 LA STRUCTURE (Doit être STRICTEMENT IDENTIQUE à celle de l'émetteur)
// // 📥 FONCTION DE RAPPEL (Callback) : Exécutée automatiquement à la réception d'un paquet
//     Serial.print("👉 ADRESSE MAC DE CE S3 : ");
//     Serial.println("❌ Erreur d'initialisation ESP-NOW");
//     Log::addLog("❌ Erreur d'initialisation ESP-NOW");
//     Serial.println("✅ ESP-NOW initialisé avec succès.");
//     Log::addLog("✅ ESP-NOW initialisé avec succès.");
//     Serial.println("📡 En écoute des signaux du télescope...");