// #include <Wire.h>
// #include <HardwareSerial.h>

// // Déclaration du GPS sur Serial2
// HardwareSerial GPS_Serial(2);

// void GPS_setup() {
//   // 1. Console de debug
//   Serial.begin(115200);

//   // 2. I2C pour BNO085
//   Wire.begin(8, 9);

//   // 3. UART pour GPS (Vitesse standard 9600 pour Neo-6M)
//   GPS_Serial.begin(9600, SERIAL_8N1, 18, 17); // RX=18, TX=17

//   Serial.println("Tous les périphériques sont assignés.");
// }