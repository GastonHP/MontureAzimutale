#pragma once
#include "../../Commun/include/Communication.h"

class Communication
{
private:
    static IMUData incomingData;

public:
    // Public methods
    static void setup(bool SSIDFound);
    static bool receive();
    static IMUData *getData();
};