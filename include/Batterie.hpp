#ifndef BATTERIE_HPP
#define BATTERIE_HPP

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

class Batterie
{
private:
    // Private members

public:
    
    // Public methods
    static float lireTension();
    static void loop(Adafruit_ST7789 *tftptr);
};

#endif // BATTERIE_HPP