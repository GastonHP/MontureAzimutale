#ifdef GMT024
#include <Wire.h>
#include <Adafruit_BNO08x.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// --- Écran ST7789 (côté gauche ESP32) ---
#define TFT_CS 27
#define TFT_DC 26
#define TFT_RST 25
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_MISO 12 // non utilisé mais requis par SPI.begin()

// --- Backlight ---
#define TFT_BL 33
#define BL_CHANNEL 0

// --- BNO085 (I2C) ---
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_BNO08x bno08x = Adafruit_BNO08x();
sh2_SensorValue_t sensorValue;

// --- Écran ---
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
  Serial.begin(115200);

  // --- PWM Backlight ---
  ledcSetup(BL_CHANNEL, 5000, 8);
  ledcAttachPin(TFT_BL, BL_CHANNEL);
  ledcWrite(BL_CHANNEL, 255);

  // --- SPI écran ---
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  // --- Init écran ---
  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  // --- Init I2C ---
  Wire.begin(SDA_PIN, SCL_PIN, 400000);

  delay(1000); // Attendre que le BNO08x soit prêt
  // --- Init BNO085 ---
  if (!bno08x.begin_I2C(0x4B, &Wire))
  {
    tft.println("BNO085 ERROR");

    while (1)
      ;
  }

  // Activer la sortie rotation (quaternions)
  bno08x.enableReport(SH2_ROTATION_VECTOR, 5000); // 5 ms
  tft.println("BNO085 OK");
  Serial.println("BNO085 OK");
}

void loop()
{
  // Lire les données IMU
  if (bno08x.getSensorEvent(&sensorValue))
  {
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
    {

      float qw = sensorValue.un.rotationVector.real;
      float qx = sensorValue.un.rotationVector.i;
      float qy = sensorValue.un.rotationVector.j;
      float qz = sensorValue.un.rotationVector.k;

      // Conversion quaternion → angles
      float yaw = atan2(2.0f * (qw * qz + qx * qy), 1 - 2 * (qy * qy + qz * qz)) * 57.2958;
      float pitch = asin(2.0f * (qw * qy - qz * qx)) * 57.2958;
      float roll = atan2(2.0f * (qw * qx + qy * qz), 1 - 2 * (qx * qx + qy * qy)) * 57.2958;

      // Affichage
      tft.setTextSize(4);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir
      //tft.fillRect(0, 0, 240, 80, ST77XX_BLACK);
      tft.setCursor(0, 40);
      tft.printf("Yaw:   %.1f\n", yaw);
      tft.printf("Pitch: %.1f\n", pitch);
      tft.printf("Roll:  %.1f\n", roll);
    }
  }
}
#endif
#ifndef GMT024
#endif