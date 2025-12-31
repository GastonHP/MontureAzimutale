// #include <TFT_eSPI.h>
// #include <SPI.h>
// #include "esp_infos.hpp"

// TFT_eSPI tft = TFT_eSPI();

// void setup() {
//   Serial.begin(115200);
//   delay(2000); // Laisse le temps au S3 de se stabiliser
//    printInfo();
//   Serial.println("Tentative d'initialisation TFT...");

//   // FORCEZ l'initialisation du bus SPI pour le S3 avant le TFT
//   SPI.begin(12, -1, 11, 10); // SCLK, MISO, MOSI, CS
  
//   tft.init(); 
//   tft.setRotation(1);
//   tft.fillScreen(TFT_BLUE);
//   Serial.println("Succès ! Écran bleu affiché.");
// }

// void loop() {}


// // #include "Arduino.h"
// // #include "esp_infos.hpp"

// // #define debug(s) Serial.println(s)

// // #include <TFT_eSPI.h>

// // TFT_eSPI tft = TFT_eSPI();
// // TFT_eSprite img = TFT_eSprite(&tft); // Création d'un tampon en mémoire vive
// // void tft_setup()
// // {
// //   Serial.begin(SERIAL_SPEED);
// //   delay(3000);
// //   debug("00");
// //   printInfo();
// //   // FORCEZ l'initialisation du bus SPI pour le S3 avant le TFT
// //   SPI.begin(12, -1, 11, 10); // SCLK, MISO, MOSI, CS
// //   debug("01");
// //   tft.init();
// //   debug("02");
// //   tft.setRotation(1);
// //   debug("03");
// //   tft.fillScreen(TFT_BLACK);
// //   debug("04");

// //   img.setColorDepth(16);
// //   debug("05");
// //   img.createSprite(TFT_WIDTH, TFT_HEIGHT); // Création du sprite de la taille de l'écran
// //   debug("06");
// //   img.fillSprite(TFT_BLUE); // Remplissage du sprite en bleu
// //   debug("07");
// //   img.setTextColor(TFT_WHITE); // Couleur du texte en blanc
// //   debug("08");
// //   img.setTextSize(2); // Taille du texte x2
// //   debug("09");
// //   img.setCursor(10, 10); // Position du curseur
// //   debug("10");
// //   img.println("TFT_eSPI Test");
// //   debug("11");
// //   img.setCursor(10, 40);
// //   debug("12");
// //   img.println("240x320 ST7789");
// //   debug("13");
// //   img.pushSprite(0, 0); // Affichage du sprite à l'écran en (0,0)
// //   debug("14");
// // }

// // #ifdef ESPC3
// // int pin = 5; // Pin por el que vamos a controlar el encedido y apagado del led
// // #endif

// // #ifdef TESTLED
// // #define LED_PIN GPIO_NUM_18

// // // the setup function runs once when you press reset or power the board
// // void LED_setup()
// // {
// //   // initialize digital pin LED_BUILTIN as an output.
// //   pinMode(LED_PIN, OUTPUT);
// // }

// // // the loop function runs over and over again forever
// // void LED_loop()
// // {
// //   digitalWrite(LED_PIN, HIGH); // turn the LED on (HIGH is the voltage level)
// //   delay(1000);                 // wait for a second
// //   digitalWrite(LED_PIN, LOW);  // turn the LED off by making the voltage LOW
// //   delay(1000);                 // wait for a second
// // }
// // #endif
// // void setup()
// // {
// //   Serial.begin(SERIAL_SPEED);
// //   delay(3000);
// //   printInfo();
// // #ifdef TESTLED
// //   LED_setup();
// // #endif
// //   tft_setup();
// // }

// // unsigned long nextTime = 0;

// // void loop()
// // {
// //   if (millis() > nextTime)
// //   {
// //     nextTime = millis() + 10000;
// //     printInfo();
// //   }
// // #ifdef TESTLED
// //   LED_loop();
// // #endif
// // }
