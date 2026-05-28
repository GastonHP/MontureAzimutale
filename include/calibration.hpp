#pragma once
#include "Arduino.h"
#include "EulerAngles.hpp"

class Calibration
{
    public:
    EulerAngles deltaAZ();
    EulerAngles deltaALT();
    
    EulerAngles origineAZ;
    EulerAngles origineALT;
    EulerAngles finALT;
    EulerAngles finAZ;
};
