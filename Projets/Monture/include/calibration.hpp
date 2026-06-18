#pragma once
#include "Arduino.h"
#include "EulerAngles.hpp"

class Calibration
{
public:
    EulerAngles deltaAZ() const;
    EulerAngles deltaALT() const;
    void reset() noexcept;

    EulerAngles origineAZ;
    EulerAngles origineALT;
    EulerAngles finAZ;
    EulerAngles finALT;
};
