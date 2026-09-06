#pragma once
#include "../../Commun/include/Communication.h"

class Communication
{
private:
    // Private members

public:
    
    // Public methods
    static void setup(bool networkHP);
    static bool send(IMUData *incomingData);
};