#pragma once
#include "../../Commun/include/Communication.h"
#include "QuickEspNow.h"

class Sender
{
public:
    static void setup(bool networkHP);
    static bool loop();
    static bool send(CapteursMessage *incomingData);
    static bool sendString(String str);
    static bool openConnection();
    static bool closeConnection();
private:
};