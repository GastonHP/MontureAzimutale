#include "Capteur.hpp"

Capteur::Capteur(Capteur::TYPE t) { typeImu = t; }

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