// #pragma once
// #include <Wire.h>
// #include <Adafruit_BNO08x.h>
// #include "EulerAngles.hpp"

// class IMU_BNO085
// {
// public:
//     struct Orientation
//     {
//         float yaw;
//         float pitch;
//         float roll;
//         float accuracyDeg;
//         bool valid;
//     };

// private:
//     static constexpr int BNO_PIN_INT = 13;
//     static constexpr int BNO_PIN_RST = 21;
//     static constexpr int BNO_I2CADDR_DEFAULT = 0x4B;
//     static constexpr int BNO_PIN_SDA = 8;
//     static constexpr int BNO_PIN_SCL = 9;

//     Adafruit_BNO08x bno = Adafruit_BNO08x(BNO_PIN_RST);

//     bool initialized = false;
//     bool yawInit = false;
//     float yawOffset = 0;

// public:
//     bool begin();
//     bool hasEvent();
//     Orientation read();
//     EulerAngles fromOrientation(const IMU_BNO085::Orientation &o);
// };
