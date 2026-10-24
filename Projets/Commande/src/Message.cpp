#include "Message.hpp"
#include "Receiver.hpp"
#include "Config.hpp"

MessageData Message::data[MAX_MESSAGE_DATA];

// MessageData Message::lastdata;
// int Message::nbDataReceived = 0;

// Message::Message(MessageType t)
// {
//     typeImu = t;
//     for (int i = 0; i < MAX_IMU_DATA; i++)
//     {
//         data[i].imuInfo.sensorId = Message::TYPE::NO_SENSOR_ID;
//     }
// }

// bool Message::areEqual(MessageData *a, MessageData *b)
// {
// #define MAX_DIFF (0.001)
//     if (abs(a->imuInfo.q_i - b->imuInfo.q_i) > MAX_DIFF)
//         return false;
//     if (abs(a->imuInfo.q_j - b->imuInfo.q_j) > MAX_DIFF)
//         return false;
//     if (abs(a->imuInfo.q_k - b->imuInfo.q_k) > MAX_DIFF)
//         return false;
//     if (abs(a->imuInfo.q_real - b->imuInfo.q_real) > MAX_DIFF)
//         return false;
//     return true;
// }

// bool Message::addImuData(MessageData *d)
// {
//     if (d->imuInfo.sensorId != typeImu)
//         return false;
//     if (areEqual(d, &data[0]) == false)
//     {
//         for (int i = MAX_IMU_DATA - 1; i > 0; i--)
//             memcpy(&data[i], &data[i - 1], sizeof(MessageData));
//     }
//     memcpy(&data[0], d, sizeof(MessageData));
//     return true;
// }

// MessageData *Message::getData(int index)
// {
//     if (index < 0 || index >= MAX_IMU_DATA)
//         return nullptr;
//     MessageData *adr = &data[index];
//     if (adr->imuInfo.sensorId != typeImu)
//         return nullptr;
//     return adr;
// }

// int Message::nbData()
// {
//     int count = 0;
//     for (int i = 0; i < MAX_IMU_DATA; i++)
//     {
//         if (data[i].imuInfo.sensorId == typeImu)
//             count++;
//         else
//             break;
//     }
//     return count;
// }

// String Message::getSummary()
// {
//     String str = "Messages summary:\n";
//     str += "Rotation: " + String(rotationMessage.nbData()) + " data points\n";
//     str += "ARVR: " + String(ARVRMessage.nbData()) + " data points\n";
//     str += "Game Rotation: " + String(gameRotationMessage.nbData()) + " data points\n";
//     return str;
// }

void Message::setup()
{
    for(int i=0; i<MAX_MESSAGE_DATA; i++)
    {
        data[i].messageType = NO_MESSAGE_TYPE;
    }
}
MessageData *Message::getMessage(MessageType messageType)
{
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].messageType == messageType)
            return &data[i];
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].messageType == NO_MESSAGE_TYPE)
        {
            data[i].messageType = messageType;
            return &data[i];
        }
    return nullptr;
}

bool Message::addMessage(MessageData *m)
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

// bool Message::saveIMUData(MessageData *m)
// {
//     memcpy(&lastdata, m, sizeof(MessageData));
//     lastdata.treated = false;
//     switch (m->imuInfo.sensorId)
//     {
//     case Message::TYPE::ARVR:
//         return ARVRMessage.addImuData(m);
//         break;
//     case Message::TYPE::ROTATION:
//         return rotationMessage.addImuData(m);
//         break;
//     case Message::TYPE::GAME_ROTATION:
//         return gameRotationMessage.addImuData(m);
//         break;
//     }
//     return false;
// }

// MessageData *Message::hasNewData()
// {
//     if (lastdata.treated == false)
//     {
//         // lastdata.treated=true;
//         return &lastdata;
//     }
//     return nullptr;
// }

// String Message::IMUDataToString(MessageData *data)
// {
//     String str = "capteursData: ";
//     str += "sent=" + String(data->sent ? "true" : "false") + ", ";
//     str += "version=" + String(data->version) + ", ";
//     str += "sent=" + String(data->sent ? "true" : "false") + ", ";
//     str += "treated=" + String(data->treated ? "true" : "false") + ", ";
//     str += "sensorId=" + String(data->imuInfo.sensorId) + ", ";
//     str += "q_real=" + String(data->imuInfo.q_real, 4) + ", ";
//     str += "q_i=" + String(data->imuInfo.q_i, 4) + ", ";
//     str += "q_j=" + String(data->imuInfo.q_j, 4) + ", ";
//     str += "q_k=" + String(data->imuInfo.q_k, 4) + ", ";

//     str += "accuracy=" + String(data->imuInfo.accuracy, 4) + ", ";
//     str += "precision=" + String(data->imuInfo.precision) + ", ";
//     str += "imu_timestamp=" + String(data->imuInfo.imu_timestamp) + ", ";
//     str += "cap_timestamp=" + String(data->imuInfo.cap_timestamp) + ", ";
//     str += "mon_timestamp=" + String(data->mon_timestamp);
//     return str;
// }

void Message::loop()
{
#ifdef MODE_TCP
    Message::nbDataReceived = Receiver::receive();
#endif
}