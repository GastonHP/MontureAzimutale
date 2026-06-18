#pragma once
#include <Arduino.h>
#include "FastAccelStepper.h"

class MotorControl
{
public:
    MotorControl(int step, int dir, int m0, int m1, int m2, int pin_enable = -1, int pin_sleep = -1);
    void begin();
    bool moveSteps(long steps); // false si déjà en mouvement, true si le mouvement a été lancé
    bool isMoving();

    void setMicrostepping(int ms);

    bool moveToDegree(float targetDeg, float ratio); // false si déjà en mouvement, true si le mouvement a été lancé
    long getPosition();

private:
    volatile bool automatique = false;

    int _stepPin;
    int _dirPin;
    int _m0, _m1, _m2;
    int _microsteps; // 1, 2, 4, 8, 16 ou 32
    int _pin_enable;
    int _pin_sleep;
    FastAccelStepper *stepper = NULL;
};