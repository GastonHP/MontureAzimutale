#pragma once
#include "../../Commun/include/Communication.h"

class Communication
{
private:
    // Private members
    static WiFiClient clientCapteur;

public:
    
    // Public methods
    static void setup(bool networkHP);
    static bool send(CapteursMessage *incomingData);
    static bool openConnection();
    static void closeConnection();
};