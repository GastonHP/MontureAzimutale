#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

// For SPI mode, we need a CS pin
#define BNO08X_CS 5
#define BNO08X_INT 7

// For SPI mode, we also need a RESET
#define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void bno08x_setup(Adafruit_ST7789 *tftptr)
{
    Wire.begin(8, 9);
    // byte error, address;
    // int nDevices;

    // Serial.println("Scanning...");

    // nDevices = 0;
    // for (address = 70; address < 127; address++)
    // {
    //     // The i2c_scanner uses the return value of
    //     // the Write.endTransmisstion to see if
    //     // a device did acknowledge to the address.
    //     Serial.print(String(address,HEX)+".");
    //     Wire.beginTransmission(address);
    //     error = Wire.endTransmission();

    //     if (error == 0)
    //     {
    //         Serial.print("I2C device found at address 0x");
    //         if (address < 16)
    //             Serial.print("0");
    //         Serial.print(address, HEX);
    //         Serial.println("  !");

    //         nDevices++;
    //     }
    //     else if (error == 4)
    //     {
    //         Serial.print("Unknown error at address 0x");
    //         if (address < 16)
    //             Serial.print("0");
    //         Serial.println(address, HEX);
    //     }
    // }
    // if (nDevices == 0)
    //     Serial.println("No I2C devices found\n");
    // else
    //     Serial.println("done\n");

    tftptr->setTextSize(2);
    tftptr->print("setup BNO!");
    // 3. Initialiser le capteur BNO085
    if (!bno08x.begin_I2C(0x4B, &Wire))
    {
        tftptr->setTextColor(ST77XX_RED);
        tftptr->setTextSize(2);
        tftptr->setCursor(10, 40);
        tftptr->print("Erreur BNO!");
        while (1)
        {
            delay(10);
        } // Stop si erreur
    }

    for (int n = 0; n < bno08x.prodIds.numEntries; n++)
    {
        Serial.print("Part ");
        Serial.print(bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
    }

    // Activer le rapport de rotation (Quaternions pour l'astronomie)
    bno08x.enableReport(SH2_ROTATION_VECTOR);

    tftptr->fillScreen(ST77XX_BLACK);
    tftptr->setTextColor(ST77XX_GREEN);
    tftptr->setTextSize(2);
    tftptr->setCursor(10, 10);
    tftptr->print("Systeme Pret");
}

void bno08x_loop(Adafruit_ST7789 *tftptr)
{
    if (bno08x.getSensorEvent(&sensorValue))
    {
        if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
        {
            // Affichage des coordonnées sur votre écran 2"
            tftptr->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            tftptr->setCursor(10, 60);
            tftptr->setTextSize(3);
            tftptr->printf("Roll: %.2f", sensorValue.un.rotationVector.real);
            // Note: Il faudra convertir les Quaternions en Degrés pour l'Az/Alt
        }
    }
}