#pragma once
#include "../../Commun/include/Communication.h"

class Sender
{
public:
    static void setup(bool networkHP);
    static bool loop();
    static bool send(MessageData *incomingData);
    static bool openConnection();
    static bool closeConnection();
private:
};