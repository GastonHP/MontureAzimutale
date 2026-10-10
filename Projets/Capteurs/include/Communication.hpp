#pragma once
#include "../../Commun/include/Communication.h"
#ifdef MODE_MAC
#include "QuickEspNow.h"
#endif

class Communication
{
private:
// Private members
#ifdef MODE_TCP
    static WiFiClient clientCapteur;
#endif

public:
    // Public methods
    static void setup(bool networkHP);
    static bool send(CapteursMessage *incomingData);
    static bool openConnection();
    static void closeConnection();

private:
#ifdef MODE_MAC
    // set the MAC address of the receiver for unicast
    static uint8_t receiver[];
#endif
};