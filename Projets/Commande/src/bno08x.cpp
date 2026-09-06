// // #pragma once
// #include "bno08x.hpp"

// // Variable volatile pour indiquer qu'une donnée est dispo (utilisée par l'interruption)
// static volatile bool bnoDataAvailable = false;

// // Fonction ultra-courte appelée en arrière-plan dès que la broche INT passe à l'état BAS
// static void IRAM_ATTR bnoISR()
// {
//     bnoDataAvailable = true;
// }

// bool IMU_BNO085::begin()
// {
//     pinMode(BNO_PIN_RST, OUTPUT);
//     digitalWrite(BNO_PIN_RST, LOW);
//     delay(10);
//     digitalWrite(BNO_PIN_RST, HIGH);
//     delay(50);

//     pinMode(BNO_PIN_INT, INPUT_PULLUP);

//     Wire.begin(BNO_PIN_SDA, BNO_PIN_SCL, 400000);

//     if (!bno.begin_I2C(BNO_I2CADDR_DEFAULT, &Wire))
//     {
//         Serial.println("BNO085 non detecte");
//         return false;
//     }

//     // Configuration de l'interruption matérielle sur l'ESP32-S3
//     // On détecte le passage du niveau HAUT au niveau BAS (FALLING)
//     pinMode(BNO_PIN_INT, INPUT_PULLUP);
//     attachInterrupt(digitalPinToInterrupt(BNO_PIN_INT), bnoISR, FALLING);

//     bno.enableReport(SH2_ROTATION_VECTOR, 20000);
//     bno.enableReport(SH2_GAME_ROTATION_VECTOR, 20000);

//     initialized = true;
//     return true;
// }

// bool IMU_BNO085::hasEvent()
// {
//     return initialized && digitalRead(BNO_PIN_INT) == LOW;
// }

// IMU_BNO085::Orientation IMU_BNO085::read()
// {
//     Orientation o{0, 0, 0, 999, false};
//     if (!initialized || !hasEvent())
//         return o;

//     sh2_SensorValue_t sensorValue;
//     if (!bno.getSensorEvent(&sensorValue))
//         return o;

//     if (sensorValue.sensorId != SH2_ROTATION_VECTOR &&
//         sensorValue.sensorId != SH2_GAME_ROTATION_VECTOR)
//         return o;

//     float qw = sensorValue.un.rotationVector.real;
//     float qx = sensorValue.un.rotationVector.i;
//     float qy = sensorValue.un.rotationVector.j;
//     float qz = sensorValue.un.rotationVector.k;

//     o.accuracyDeg = sensorValue.un.rotationVector.accuracy * 57.2958f;
//     if (o.accuracyDeg > 6.0f)
//         return o;

//     float ysqr = qy * qy;

//     float t0 = +2.0f * (qw * qx + qy * qz);
//     float t1 = +1.0f - 2.0f * (qx * qx + ysqr);
//     o.roll = atan2(t0, t1) * 57.2958f;

//     float t2 = +2.0f * (qw * qy - qz * qx);
//     t2 = t2 > 1.0f ? 1.0f : t2;
//     t2 = t2 < -1.0f ? -1.0f : t2;
//     o.pitch = asin(t2) * 57.2958f;

//     float t3 = +2.0f * (qw * qz + qx * qy);
//     float t4 = +1.0f - 2.0f * (ysqr + qz * qz);
//     o.yaw = atan2(t3, t4) * 57.2958f;

//     if (!yawInit)
//     {
//         yawOffset = o.yaw;
//         yawInit = true;
//     }
//     o.yaw -= yawOffset;
//     if (o.yaw < 0)
//         o.yaw += 360.0f;
//     if (o.yaw >= 360)
//         o.yaw -= 360.0f;

//     o.valid = true;
//     return o;
// }

// EulerAngles IMU_BNO085::fromOrientation(const IMU_BNO085::Orientation &o)
// {
//     return EulerAngles(o.yaw, o.pitch, o.roll);
// }