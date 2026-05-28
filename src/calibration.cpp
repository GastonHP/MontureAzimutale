#include "calibration.hpp"

EulerAngles Calibration::deltaAZ()
{
    return finAZ - origineAZ;
}

EulerAngles Calibration::deltaALT()
{
    return finALT - origineALT;
}