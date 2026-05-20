
#include <Wire.h>
#ifdef ADAFRUIT_BNO085
#include <Adafruit_BNO08x.h>
#endif
#ifdef SPARKFUN_BNO085
#include <SparkFun_BNO08x_Arduino_Library.h>
#endif
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#ifdef USE_TEMP_SENSOR
#include <DallasTemperature.h>
// --- DS18B20 ---
#define ONE_WIRE_BUS 32
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#endif
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
#define BNO_ADDR 0x4B

#ifdef ADAFRUIT_BNO085
Adafruit_BNO08x bno08x = Adafruit_BNO08x();
sh2_SensorValue_t sensorValue;
#endif

#ifdef SPARKFUN_BNO085
BNO08x bno08x;
#endif

// --- Écran ---
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// // --- Buffers pour moyenne glissante ---
// float yawBuf[5] = {0};
// float pitchBuf[5] = {0};
// float rollBuf[5] = {0};

// float smooth(float newValue, float *buffer, int size)
// {
//   for (int i = size - 1; i > 0; i--)
//     buffer[i] = buffer[i - 1];
//   buffer[0] = newValue;

//   float sum = 0;
//   for (int i = 0; i < size; i++)
//     sum += buffer[i];

//   return sum / size;
// }

static int cursorY = 22; // Position verticale de départ pour l'affichage des données IMU

void setup()
{
  Serial.begin(115200);

  // --- PWM Backlight ---
  ledcSetup(BL_CHANNEL, 5000, 8);
  ledcAttachPin(TFT_BL, BL_CHANNEL);
  ledcWrite(BL_CHANNEL, 63);

  // --- SPI écran ---
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);

  // --- Init écran ---
  tft.init(240, 320);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir
  tft.setTextSize(2);
  tft.setCursor(0, 0);

  // --- Init I2C ---
  Wire.begin(SDA_PIN, SCL_PIN, 400000);

#ifdef ADAFRUIT_BNO085
  // delay(1000); // Attendre que le BNO08x soit prêt
  //  --- Init BNO085 ---
  if (!bno08x.begin_I2C(BNO_ADDR, &Wire))
  {
    tft.println("BNO085 ERROR");
    while (1)
      ;
  }

  // Précision maximale : RV + GRV à 50 Hz
  bno08x.enableReport(SH2_ROTATION_VECTOR);
  // bno08x.enableReport(SH2_GAME_ROTATION_VECTOR);
  bno08x.enableReport(SH2_ARVR_STABILIZED_RV, 5000);
  tft.println("AdaFruit BNO085 OK");
  Serial.println("BNO085 OK");
#endif
#ifdef SPARKFUN_BNO085
  if (!bno08x.begin(BNO_ADDR, Wire))
  {
    tft.println("BNO085 ERROR");

    while (1)
      ;
  }
  bno08x.enableRotationVector(50);
  // bno08x.enableGameRotationVector(10);
  bno08x.enableARVRStabilizedRotationVector(50);
  tft.println("SPARKFUN BNO085 OK");
  Serial.println("BNO085 OK");
#endif
tft.fillRect(0, cursorY-4, 240, 3, ST77XX_RED);
#ifdef USE_TEMP_SENSOR
  // --- Init DS18B20 ---
  sensors.begin();
#endif
}

void gen_loop()
{
#ifdef USE_TEMP_SENSOR
  static unsigned long lastTempRead = 0;
  if (millis() - lastTempRead > 2000) // Lire la température toutes les 2 secondes
  {
    lastTempRead = millis();
    // --- Lire température DS18B20 ---
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    tft.setCursor(0, 280);
    tft.setTextSize(2);
    tft.printf("Temp: %.2fC  ", tempC);
  }
#endif
}

#ifdef ADAFRUIT_BNO085
void loop()
{
  // Lire les données IMU
  
  if (bno08x.getSensorEvent(&sensorValue))
  {
    if (sensorValue.sensorId == SH2_ARVR_STABILIZED_RV)
    {
      // On récupère le statut global de la précision
      // sensorValue.status contient la valeur (0 à 3)
      uint8_t precision = sensorValue.status & 0x03;
      // À ajouter dans votre fonction afficherInterface
      float accuracy = sensorValue.un.arvrStabilizedRV.accuracy * 57.2958; // 0 = Invalide, 3 = Précision max
      float qr = sensorValue.un.arvrStabilizedRV.real;
      float qi = sensorValue.un.arvrStabilizedRV.i;
      float qj = sensorValue.un.arvrStabilizedRV.j;
      float qk = sensorValue.un.arvrStabilizedRV.k;

      // Conversion quaternion → angles
      float yaw = 90.0 + atan2(2.0f * (qr * qk + qi * qj), 1 - 2 * (qj * qj + qk * qk)) * 57.2958;
      float pitch = asin(2.0f * (qr * qj - qk * qi)) * 57.2958;
      float roll = atan2(2.0f * (qr * qi + qj * qk), 1 - 2 * (qi * qi + qj * qj)) * 57.2958;

      tft.setCursor(0, cursorY);
      tft.setTextSize(1);
      tft.printf("ARVR Stabilized RV\n");
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir
      // tft.fillRect(0, 0, 240, 80, ST77XX_BLACK);

      tft.printf("Yaw:   %.1f   \n", yaw);
      tft.printf("Pitch: %.1f   \n", pitch);
      tft.printf("Roll:  %.1f   \n", roll);
      tft.printf("Acc:  %.1f ", accuracy);
      tft.printf("Prec: %d \n", precision);
    }
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR)
    {
      // On récupère le statut global de la précision
      // sensorValue.status contient la valeur (0 à 3)
      uint8_t precision = sensorValue.status & 0x03;
      // À ajouter dans votre fonction afficherInterface
      float accuracy = sensorValue.un.rotationVector.accuracy * 57.2958; // 0 = Invalide, 3 = Précision max
      float qr = sensorValue.un.rotationVector.real;
      float qi = sensorValue.un.rotationVector.i;
      float qj = sensorValue.un.rotationVector.j;
      float qk = sensorValue.un.rotationVector.k;

      // Conversion quaternion → angles
      float yaw = 90.0 + atan2(2.0f * (qr * qk + qi * qj), 1 - 2 * (qj * qj + qk * qk)) * 57.2958;
      float pitch = asin(2.0f * (qr * qj - qk * qi)) * 57.2958;
      float roll = atan2(2.0f * (qr * qi + qj * qk), 1 - 2 * (qi * qi + qj * qj)) * 57.2958;

      tft.setCursor(0, cursorY + 80);
      tft.setTextSize(1);
      tft.printf("Rotation Vector\n");
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir
      // tft.fillRect(0, 0, 240, 80, ST77XX_BLACK);

      tft.printf("Yaw:   %.1f   \n", yaw);
      tft.printf("Pitch: %.1f   \n", pitch);
      tft.printf("Roll:  %.1f   \n", roll);
      tft.printf("Acc:  %.1f ", accuracy);
      tft.printf("Prec: %d \n", precision);
    }
    if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR)
    {
      // On récupère le statut global de la précision
      // sensorValue.status contient la valeur (0 à 3)
      uint8_t precision = sensorValue.status & 0x03;
      // À ajouter dans votre fonction afficherInterface
      // float accuracy = sensorValue.un.gameRotationVector. * 57.2958; // 0 = Invalide, 3 = Précision max
      float qr = sensorValue.un.gameRotationVector.real;
      float qi = sensorValue.un.gameRotationVector.i;
      float qj = sensorValue.un.gameRotationVector.j;
      float qk = sensorValue.un.gameRotationVector.k;

      // Conversion quaternion → angles
      float yaw = atan2(2.0f * (qr * qk + qi * qj), 1 - 2 * (qj * qj + qk * qk)) * 57.2958;
      float pitch = asin(2.0f * (qr * qj - qk * qi)) * 57.2958;
      float roll = atan2(2.0f * (qr * qi + qj * qk), 1 - 2 * (qi * qi + qj * qj)) * 57.2958;

      tft.setCursor(0, cursorY + 160);
      tft.setTextSize(1);
      tft.printf("Game Rotation Vector\n");
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir
      // tft.fillRect(0, 0, 240, 80, ST77XX_BLACK);

      tft.printf("Yaw:   %.1f   \n", yaw);
      tft.printf("Pitch: %.1f   \n", pitch);
      tft.printf("Roll:  %.1f   \n", roll);
      // tft.printf("Acc:   %.1f   \n", accuracy);
      tft.printf("Prec:  %d\n", precision);
    }
  }
  gen_loop();
}
#endif
#ifdef SPARKFUN_BNO085
void loop()
{
  if (bno08x.getSensorEvent() == true)
  {
    if (bno08x.getSensorEventID() == SH2_ARVR_STABILIZED_RV)
    {
      float qr = bno08x.getQuatReal();
      float qi = bno08x.getQuatI();
      float qj = bno08x.getQuatJ();
      float qk = bno08x.getQuatK();

      float accuracy = bno08x.getQuatRadianAccuracy() * 57.2958;
      uint8_t precision = bno08x.getQuatAccuracy();

      // Conversion quaternion → angles
      float yaw = 90.0 + atan2(2.0f * (qr * qk + qi * qj), 1 - 2 * (qj * qj + qk * qk)) * 57.2958;
      float pitch = asin(2.0f * (qr * qj - qk * qi)) * 57.2958;
      float roll = atan2(2.0f * (qr * qi + qj * qk), 1 - 2 * (qi * qi + qj * qj)) * 57.2958;

      tft.setCursor(0, cursorY);
      tft.setTextSize(1);
      tft.printf("ARVR Stabilized RV\n");
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir

      tft.printf("Yaw:  %.1f   \n", yaw);
      tft.printf("Pitch:%.1f   \n", pitch);
      tft.printf("Roll: %.1f   \n", roll);
      tft.printf("Acc:  %.1f ", accuracy);
      tft.printf("Prec:%d  \n", precision);
    }
    if (bno08x.getSensorEventID() == SH2_ROTATION_VECTOR)
    {
      float roll = bno08x.getRoll() * 180.0f / PI;
      float pitch = bno08x.getPitch() * 180.0f / PI;
      float yaw = 90.0 + bno08x.getYaw() * 180.0f / PI;

      float accuracy = bno08x.getQuatRadianAccuracy() * 57.2958;
      uint8_t precision = bno08x.getQuatAccuracy();

      tft.setCursor(0, cursorY + 80);
      tft.setTextSize(1);
      tft.printf("Rotation Vector\n");
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK); // Texte blanc sur fond noir

      tft.printf("Yaw:  %.1f   \n", yaw);
      tft.printf("Pitch:%.1f   \n", pitch);
      tft.printf("Roll: %.1f   \n", roll);
      tft.printf("Acc:  %.1f ", accuracy);
      tft.printf("Prec:%d  \n", precision);
    }
  }
  gen_loop();
}
#endif