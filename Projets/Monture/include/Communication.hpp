#pragma once
#include "../../Commun/include/Communication.h"

class Communication
{
private:
    static struct_message incomingData;

public:
    // Public methods
    static void setup(bool SSIDFound);
    static bool receive();
    static struct_message *getData();
};