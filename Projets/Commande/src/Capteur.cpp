#include "Capteur.hpp"
#include "Receiver.hpp"
#include "Config.hpp"

MessageData Capteur::lastdata;
Capteur Capteur::rotationCapteur = Capteur(Capteur::TYPE::ROTATION);
Capteur Capteur::ARVRCapteur = Capteur(Capteur::TYPE::ARVR);
Capteur Capteur::gameRotationCapteur = Capteur(Capteur::TYPE::GAME_ROTATION);
int Capteur::nbDataReceived = 0;

Capteur::Capteur(Capteur::TYPE t)
{
    typeImu = t;
    for (int i = 0; i < MAX_IMU_DATA; i++)
    {
        data[i].imuInfo.sensorId = Capteur::TYPE::NO_SENSOR_ID;
    }
}

bool Capteur::areEqual(MessageData *a, MessageData *b)
{
#define MAX_DIFF (0.001)
    if (abs(a->imuInfo.q_i - b->imuInfo.q_i) > MAX_DIFF)
        return false;
    if (abs(a->imuInfo.q_j - b->imuInfo.q_j) > MAX_DIFF)
        return false;
    if (abs(a->imuInfo.q_k - b->imuInfo.q_k) > MAX_DIFF)
        return false;
    if (abs(a->imuInfo.q_real - b->imuInfo.q_real) > MAX_DIFF)
        return false;
    return true;
}

bool Capteur::addImuData(MessageData *d)
{
    if (d->imuInfo.sensorId != typeImu)
        return false;
    if (areEqual(d, &data[0]) == false)
    {
        for (int i = MAX_IMU_DATA - 1; i > 0; i--)
            memcpy(&data[i], &data[i - 1], sizeof(MessageData));
    }
    memcpy(&data[0], d, sizeof(MessageData));
    return true;
}

MessageData *Capteur::getData(int index)
{
    if (index < 0 || index >= MAX_IMU_DATA)
        return nullptr;
    MessageData *adr = &data[index];
    if (adr->imuInfo.sensorId != typeImu)
        return nullptr;
    return adr;
}

int Capteur::nbData()
{
    int count = 0;
    for (int i = 0; i < MAX_IMU_DATA; i++)
    {
        if (data[i].imuInfo.sensorId == typeImu)
            count++;
        else
            break;
    }
    return count;
}

String Capteur::getSummary()
{
    String str = "Capteurs summary:\n";
    str += "Rotation: " + String(rotationCapteur.nbData()) + " data points\n";
    str += "ARVR: " + String(ARVRCapteur.nbData()) + " data points\n";
    str += "Game Rotation: " + String(gameRotationCapteur.nbData()) + " data points\n";
    return str;
}

void Capteur::setup()
{
    
}

bool Capteur::saveIMUData(MessageData *m)
{
    memcpy(&lastdata, m, sizeof(MessageData));
    lastdata.treated = false;
    switch (m->imuInfo.sensorId)
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

MessageData *Capteur::hasNewData()
{
    if (lastdata.treated == false)
    {
        // lastdata.treated=true;
        return &lastdata;
    }
    return nullptr;
}

String Capteur::IMUDataToString(MessageData *data)
{
    String str = "capteursData: ";
    str += "sent=" + String(data->sent ? "true" : "false") + ", ";
    str += "version=" + String(data->version) + ", ";
    str += "sent=" + String(data->sent ? "true" : "false") + ", ";
    str += "treated=" + String(data->treated ? "true" : "false") + ", ";
    str += "sensorId=" + String(data->imuInfo.sensorId) + ", ";
    str += "q_real=" + String(data->imuInfo.q_real, 4) + ", ";
    str += "q_i=" + String(data->imuInfo.q_i, 4) + ", ";
    str += "q_j=" + String(data->imuInfo.q_j, 4) + ", ";
    str += "q_k=" + String(data->imuInfo.q_k, 4) + ", ";

    str += "accuracy=" + String(data->imuInfo.accuracy, 4) + ", ";
    str += "precision=" + String(data->imuInfo.precision) + ", ";
    str += "imu_timestamp=" + String(data->imuInfo.imu_timestamp) + ", ";
    str += "cap_timestamp=" + String(data->imuInfo.cap_timestamp) + ", ";
    str += "mon_timestamp=" + String(data->mon_timestamp);
    return str;
}

void Capteur::loop()
{
#ifdef MODE_TCP
    Capteur::nbDataReceived = Receiver::receive();
#endif
}