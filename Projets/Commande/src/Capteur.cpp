#include "Capteur.hpp"
#include "Communication.hpp"
#include "Config.hpp"

CapteursMessage Capteur::lastdata;
Capteur Capteur::rotationCapteur = Capteur(Capteur::TYPE::ROTATION);
Capteur Capteur::ARVRCapteur = Capteur(Capteur::TYPE::ARVR);
Capteur Capteur::gameRotationCapteur = Capteur(Capteur::TYPE::GAME_ROTATION);
int Capteur::nbDataReceived = 0;

Capteur::Capteur(Capteur::TYPE t)
{
    typeImu = t;
    for (int i = 0; i < MAX_IMU_DATA; i++)
    {
        data[i].sensorId = Capteur::TYPE::NO_SENSOR_ID;
    }
}

bool Capteur::areEqual(CapteursMessage *a, CapteursMessage *b)
{
#define MAX_DIFF (0.001)
    if (abs(a->q_i - b->q_i) > MAX_DIFF)
        return false;
    if (abs(a->q_j - b->q_j) > MAX_DIFF)
        return false;
    if (abs(a->q_k - b->q_k) > MAX_DIFF)
        return false;
    if (abs(a->q_real - b->q_real) > MAX_DIFF)
        return false;
    return true;
}

bool Capteur::addImuData(CapteursMessage *d)
{
    if (d->sensorId != typeImu)
        return false;
    if (areEqual(d, &data[0]) == false)
    {
        for (int i = MAX_IMU_DATA - 1; i > 0; i--)
            memcpy(&data[i], &data[i - 1], sizeof(CapteursMessage));
    }
    memcpy(&data[0], d, sizeof(CapteursMessage));
    return true;
}

CapteursMessage *Capteur::getData(int index)
{
    if (index < 0 || index >= MAX_IMU_DATA)
        return nullptr;
    CapteursMessage *adr = &data[index];
    if (adr->sensorId != typeImu)
        return nullptr;
    return adr;
}

int Capteur::nbData()
{
    int count = 0;
    for (int i = 0; i < MAX_IMU_DATA; i++)
    {
        if (data[i].sensorId == typeImu)
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
    Communication::setup(Config::NetworkHP());
}

bool Capteur::saveIMUData(CapteursMessage *m)
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

CapteursMessage *Capteur::hasNewData()
{
    if (lastdata.treated == false)
    {
        // lastdata.treated=true;
        return &lastdata;
    }
    return nullptr;
}

String Capteur::IMUDataToString(CapteursMessage *data)
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

void Capteur::loop()
{
    Capteur::nbDataReceived = Communication::receive();
}