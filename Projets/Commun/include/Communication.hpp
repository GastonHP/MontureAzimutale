#pragma once
#include "../../Commun/include/Communication.h"
#include "Message.hpp"

class Communication
{
public:
    static void setup(bool networkHP);
    static bool loop();
    static bool send(MessageData *incomingData);
    // static bool openConnection();
    // static bool closeConnection();
private:
static MessageData buffer;
};