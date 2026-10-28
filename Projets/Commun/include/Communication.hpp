#pragma once
#include "Message.hpp"


#define udpLocalPort 4210 // Port d'écoute local

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