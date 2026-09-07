#pragma once
#include "../../Commun/include/Communication.h"

class Communication
{
private:
    static CapteursMessage incomingData;

public:
    // Public methods
    static void setup(bool SSIDFound);
    static bool receive();
    static CapteursMessage *getData();
    static String error;
};