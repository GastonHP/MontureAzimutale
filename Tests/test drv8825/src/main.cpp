#include "Arduino.h"

// Définition des broches
const int stepPin = 3;
const int dirPin = 2;
const int sleepPin = 4;
const int enablePin = 5; // Si votre driver a une broche d'activation, sinon ignorez cette ligne

#ifndef FAS

#define VITESSE 5000 // Vitesse de rotation en microsecondes (plus petit = plus rapide)
void setup()
{
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);

  digitalWrite(sleepPin, HIGH); // Réveille le driver
  Serial.begin(9600);
  Serial.println("Test Moteur 42BYG304 lancé...");
}

void loop()
{
  // --- SENS HORAIRE ---
  digitalWrite(dirPin, HIGH);
  Serial.println("Rotation Horaire");
  for (int x = 0; x < 200; x++)
  { // 200 pas = 1 tour en Full Step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(VITESSE); // Vitesse : plus le chiffre est petit, plus c'est rapide
    digitalWrite(stepPin, LOW);
    delayMicroseconds(VITESSE);
  }
  delay(1000); // Pause d'une seconde

  // --- SENS ANTI-HORAIRE ---
  digitalWrite(dirPin, LOW);
  Serial.println("Rotation Anti-Horaire");
  for (int x = 0; x < 200; x++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(VITESSE); // On teste un peu plus vite
    digitalWrite(stepPin, LOW);
    delayMicroseconds(VITESSE);
  }
  delay(1000);
}
#else
#include "FastAccelStepper.h"
#include "AVRStepperPins.h" // Only required for AVR controllers

// If using an AVR device use the definitons provided in AVRStepperPins
//    stepPinStepper1A
//
// or even shorter (for 2560 the correct pin on the chosen timer is selected):
//    stepPinStepperA

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

void setup() {
   engine.init();
   stepper = engine.stepperConnectToPin(stepPin);
   if (stepper) {
      stepper->setDirectionPin(dirPin);
      stepper->setEnablePin(enablePin, false); // low active
      stepper->setAutoEnable(true);

      stepper->setSpeedInHz(500);       // 500 steps/s
      stepper->setAcceleration(100);    // 100 steps/s²
      stepper->move(1000);
   }
}

void loop() {
}
#endif