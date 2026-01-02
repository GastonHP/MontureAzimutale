#include "generic.hpp"
#include "esp_infos.hpp"

extern void lcd_test_setup(Adafruit_ST7789 *monEcran);

void setup()
{
  Serial.begin(SERIAL_SPEED);
  delay(2000); // Laisse le temps au S3 de se stabiliser
  // printInfo();

  monEcran_setup();
  delay(500);
  bno08x_setup(&monEcran);
}

unsigned long nextTime = 0;

void loop()
{
  // if (millis() > nextTime)
  // {
  //   nextTime = millis() + 10000;
  //   printInfo();
  // }
  bno08x_loop(&monEcran);
}