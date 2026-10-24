#include "Message.hpp"
#include "Config.hpp"

MessageData Message::data[MAX_MESSAGE_DATA];

void Message::setup()
{
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
    {
        data[i].messageType = NO_MESSAGE_TYPE;
    }
}

MessageData *Message::getMessageByIndex(int index)
{
    if (index < MAX_MESSAGE_DATA)
        return &data[index];
    return nullptr;
}

MessageData *Message::getSendMessage(MessageType messageType)
{
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].messageType == messageType && (data[i].status == ToSend || data[i].status == Sent))
            return &data[i];
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].messageType == NO_MESSAGE_TYPE)
        {
            data[i].messageType = messageType;
            return &data[i];
        }
    return nullptr;
}

MessageData *Message::getReceiveMessage(MessageType messageType)
{
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].messageType == messageType && (data[i].status == Received || data[i].status == Treated))
            return &data[i];
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].messageType == NO_MESSAGE_TYPE)
        {
            data[i].messageType = messageType;
            return &data[i];
        }
    return nullptr;
}

MessageData *Message::getMessageToSend()
{
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].status == ToSend)
            return &data[i];
    return nullptr;
}

MessageData *Message::getMessageToTreat()
{
    for (int i = 0; i < MAX_MESSAGE_DATA; i++)
        if (data[i].status == Received)
            return &data[i];
    return nullptr;
}

bool Message::addMessageToSend(MessageData *m)
{
    MessageData *dest = getSendMessage(m->messageType);
    if (dest == nullptr)
        return false;
    memcpy(dest, m, sizeof(MessageData));
    dest->status = ToSend;
    return true;
}

bool Message::addReceivedMessage(MessageData *m)
{
    MessageData *dest = getReceiveMessage(m->messageType);
    if (dest == nullptr)
        return false;
    memcpy(dest, m, sizeof(MessageData));
    dest->status = Received;
    return true;
}

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