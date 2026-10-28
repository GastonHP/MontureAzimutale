#pragma once
#include "../../../Projets/Commun/include/Communication.hpp"
#include "WiFiUdp.h"

class Receiver
{
private:
public:
    // Public methods
    static void setup(bool SSIDFound);
    static void loop();
    static String error;
};