#include "calibration.hpp"

EulerAngles Calibration::deltaAZ() const
{
    return finAZ - origineAZ;
}

EulerAngles Calibration::deltaALT() const
{
    return finALT - origineALT;
}

void Calibration::reset() noexcept
{
    origineAZ = EulerAngles();
    origineALT = EulerAngles();
    finAZ = EulerAngles();
    finALT = EulerAngles();
}