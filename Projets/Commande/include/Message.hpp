#pragma once
#include "Arduino.h"
#include <Adafruit_BNO08x.h>
#include "../../Commun/include/Communication.h"

// pour chaque capteur, on garde les 20 dernières valeurs reçues
#define MAX_MESSAGE_DATA 20

class Message
{
public:
    //bool addImuData(MessageData *data);
    //MessageData *getData(int index = 0);
    //int nbData();

    static void setup();
    static void loop();
    static MessageData *getMessage(MessageType messageType);
    static bool addMessage(MessageData *m);
    //static bool saveIMUData(MessageData *m);
    //static MessageData *hasNewData();

    //static String IMUDataToString(MessageData *data);
    //static String getSummary();

private:
    //Message::TYPE typeImu;
    static MessageData data[MAX_MESSAGE_DATA];
    //static bool areEqual(MessageData *a, MessageData *b);

    // static MessageData lastdata;
    // static int nbDataReceived;
};