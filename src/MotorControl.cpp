#include <Arduino.h>
#include "MotorControl.hpp"

static FastAccelStepperEngine *engine = nullptr;

MotorControl::MotorControl(int step, int dir, int m0, int m1, int m2, int pin_enable, int pin_sleep)
{
    this->_stepPin = step;
    this->_dirPin = dir;
    this->_m0 = m0;
    this->_m1 = m1;
    this->_m2 = m2;
    this->_pin_enable = pin_enable;
    this->_pin_sleep = pin_sleep;
    this->_microsteps = 32; // Par défaut pour le suivi astro
}

void MotorControl::begin()
{
    // 1. Configuration du Microstepping (ex: 1/16)
    pinMode(_m0, OUTPUT);
    pinMode(_m1, OUTPUT);
    pinMode(_m2, OUTPUT);
    pinMode(_pin_sleep, OUTPUT);
    digitalWrite(_pin_sleep, HIGH); // Sortir du mode sommeil

    // 2. Initialisation du moteur de calcul (Engine)
    if (engine == nullptr)
    {
        engine = new FastAccelStepperEngine();
        engine->init();
    }
    // 3. Configuration de l'Azimut (Moteur 1)
    stepper = engine->stepperConnectToPin(_stepPin);
    if (stepper)
    {
        stepper->setDirectionPin(_dirPin);
        stepper->setEnablePin(_pin_enable);
        stepper->setAutoEnable(false); // Active le driver seulement lors du mouvement
        stepper->enableOutputs(); // Active le driver (si autoEnable est false)
        

        stepper->setSpeedInHz(3200);    // Vitesse max (pas/sec)
        stepper->setAcceleration(1000); // Rampe d'accélération
        setMicrostepping(_microsteps);  // Configurer le microstepping
    }
}

bool MotorControl::moveSteps(long steps)
{
    stepper->move(steps);
    return true;
}

// Définit la résolution (1/1 à 1/32)
void MotorControl::setMicrostepping(int ms)
{
    _microsteps = ms;
    bool m0_state;
    bool m1_state;
    bool m2_state;

    switch (_microsteps)
    {
    case 1:
        m0_state = LOW;
        m1_state = LOW;
        m2_state = LOW;
        break;
    case 2:
        m0_state = HIGH;
        m1_state = LOW;
        m2_state = LOW;
        break;
    case 4:
        m0_state = LOW;
        m1_state = HIGH;
        m2_state = LOW;
        break;
    case 8:
        m0_state = HIGH;
        m1_state = HIGH;
        m2_state = LOW;
        break;
    case 16:
        m0_state = LOW;
        m1_state = LOW;
        m2_state = HIGH;
        break;
    case 32:
    default:
        m0_state = HIGH;
        m1_state = LOW;
        m2_state = HIGH;
    }

    digitalWrite(_m0, m0_state);
    digitalWrite(_m1, m1_state);
    digitalWrite(_m2, m2_state);
}

// GoTo relatif en degrés (basé sur un rapport de réduction)
// Rapport AZ-Pronto approx 1:120 (à ajuster)
bool MotorControl::moveToDegree(float targetDeg, float ratio)
{
    if (isMoving())
        return false; // Ne pas lancer un nouveau mouvement si déjà en mouvement
    long targetSteps = (targetDeg * 200 * _microsteps * ratio) / 360.0;
    long diff = targetSteps - getPosition();
    return moveSteps(diff);
}

long MotorControl::getPosition()
{
    return stepper->getCurrentPosition();
}

bool MotorControl::isMoving()
{
    // FastAccelStepper gère déjà l'état de mouvement, on peut s'y fier
    return stepper->isRunning();
}
