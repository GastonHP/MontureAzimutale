#include "Keyboard.hpp"

#include <NimBLEDevice.h>

// UUIDs standards pour le service UART (Nordic)
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

NimBLEServer* pServer = NULL;
NimBLECharacteristic* pTxCharacteristic = NULL;
bool deviceConnected = false;

// Classe pour gérer la réception des messages
class MyCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            char cmd = rxValue[0]; // On prend le premier caractère
            Serial.printf("Commande reçue : %c\n", cmd);
            
            // --- VOTRE LOGIQUE MOTEUR ICI ---
            // if(cmd == '8') stepperAl->runForward();
            // if(cmd == '5') { stepperAz->stopMove(); stepperAl->stopMove(); }
            // ... etc
        }
    }
};

void setupBLE() {
  NimBLEDevice::init("Telescope_S3_BLE");
  pServer = NimBLEDevice::createServer();
  
  // Création du service UART
  NimBLEService *pService = pServer->createService(SERVICE_UUID);
  
  // Caractéristique pour ENVOYER des données vers le téléphone (TX)
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY);
  
  // Caractéristique pour RECEVOIR des données du téléphone (RX)
  NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, NIMBLE_PROPERTY::WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("BLE prêt et en attente de connexion...");
}

char Keyboard::lireTouche()
{
    // Implementation to read a key from the keyboard
    // This is a placeholder implementation and should be replaced with actual code to read from the hardware
    return 'A'; // Return a dummy character for testing
}

void Keyboard::loop()
{
    // Implementation of the loop function to continuously read from the keyboard
    // This is a placeholder implementation and should be replaced with actual code to read from the hardware
    char key = lireTouche();
    // Process the key as needed
}

void Keyboard::setup()
{
    // Implementation of the setup function to initialize the keyboard
    // This is a placeholder implementation and should be replaced with actual code to initialize the hardware
    // 1. Bluetooth
    NimBLEDevice::init("Telescope_S3");
    NimBLEServer *pServer = NimBLEDevice::createServer();
}

NimBLEClient *Keyboard::pClient;