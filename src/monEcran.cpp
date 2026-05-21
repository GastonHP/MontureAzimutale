#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include "Log.hpp"
#include "monEcran.hpp"
#include "Batterie.hpp"
#include "GPSManager.hpp"
#include "Telescope.hpp"

// Define pins for the TFT display
#define TFT_CS 10   // Chip select pin
#define TFT_DC 4    // 13   // Data Command pin
#define TFT_RST 14  // Reset pin (could connect to Arduino RESET pin)
#define TFT_MOSI 11 // Data out
#define TFT_MISO 13 // Data in
#define TFT_SCLK 12 // Clock out
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_ROTATION 2

static Adafruit_ST7789 monEcran = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void MonEcran::setup()
{
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI); // SCK, MISO, MOSI

    monEcran.init(TFT_WIDTH, TFT_HEIGHT);
    monEcran.setRotation(TFT_ROTATION);
    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    // monEcran.setSPISpeed(40000000);
    Serial.println(F("monEcran initialized"));
    Log::addLog("monEcran initialized");
    uint16_t time = millis();
    Serial.print("remplissage en noir...");
    monEcran.fillScreen(ST77XX_BLACK);
    Log::addLog("monEcran fillScreen done in " + String(millis() - time, DEC) + "ms");
    Serial.println(" en " + String(millis() - time, DEC) + "ms.");
}

void MonEcran::log(String s)
{
    monEcran.setTextSize(1);
    monEcran.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    monEcran.println(s);
}

void MonEcran::logError(String s)
{
    monEcran.setTextSize(2);
    monEcran.setTextColor(ST77XX_RED, ST77XX_BLACK);
    monEcran.println(s);
}

void MonEcran::afficherBoussole(EulerAngles angles)
{
    // Affichage de la boussole
    float azimut = angles.yaw; // Azimut magnétique

    uint16_t couleurAiguille = ST77XX_BLUE;
    azimut = Telescope::calculerAzimutVrai(azimut);
    couleurAiguille = ST77XX_GREEN;

    int cX = 80;  // Centre X
    int cY = 160; // Centre Y
    int r = 80;   // Rayon du cadran

    // 1. Dessiner le cadran fixe (une seule fois idéalement)
    monEcran.drawCircle(cX, cY, r, ST77XX_WHITE);
    monEcran.setTextColor(ST77XX_RED);
    monEcran.setCursor(cX - 5, cY - r - 15);
    monEcran.print("N");
    monEcran.setTextColor(ST77XX_WHITE);
    monEcran.setCursor(cX - 5, cY + r + 5);
    monEcran.print("S");
    // 2. Calculer l'angle de l'aiguille (en radians)
    float rad = azimut * PI / 180.0;

    // 3. Effacer l'ancienne aiguille (méthode simple : redessiner le fond)
    static float lastRad = 0;
    monEcran.drawLine(cX, cY, cX + sin(lastRad) * r, cY - cos(lastRad) * r, ST77XX_BLACK);

    // 4. Dessiner la nouvelle aiguille
    // Nord en Rouge
    monEcran.drawLine(cX, cY, cX + sin(rad) * r, cY - cos(rad) * r, couleurAiguille);
    // Sud en Blanc (optionnel)
    monEcran.drawLine(cX, cY, cX - sin(rad) * (r / 2), cY + cos(rad) * (r / 2), ST77XX_WHITE);

    lastRad = rad;

    // 5. Affichage numérique au centre
    monEcran.fillRect(cX - 20, cY + 20, 40, 15, ST77XX_BLACK); // Effacement partiel
    monEcran.setCursor(cX - 15, cY + 20);
    monEcran.printf("%03d", (int)azimut);
}

void MonEcran::afficherNiveauBulle(EulerAngles angles)
{
    // Affichage du niveau à bulle
    static int oldX = 80;
    static int oldY = 160;
    int centreX = 80;
    int centreY = 160;
    int sensibilite = 8; // Ajustez pour rendre la bulle plus ou moins vive

    // 1. Calcul de la nouvelle position (Inversion du signe selon l'orientation du capteur)
    int newX = centreX + (int)(angles.roll * sensibilite);
    int newY = centreY + (int)(angles.pitch * sensibilite);

    // 2. Contrainte pour que la bulle ne sorte pas trop de la cible
    newX = constrain(newX, centreX - 60, centreX + 60);
    newY = constrain(newY, centreY - 60, centreY + 60);

    // 3. Dessin de la cible fixe (une seule fois ou à chaque tour)
    monEcran.drawCircle(centreX, centreY, 50, ST77XX_WHITE);
    monEcran.drawCircle(centreX, centreY, 5, ST77XX_WHITE); // Point central

    // 4. Effacer l'ancienne bulle (en la dessinant en noir)
    if (newX != oldX || newY != oldY)
    {
        monEcran.fillCircle(oldX, oldY, 10, ST77XX_BLACK);
    }

    // 5. Dessiner la nouvelle bulle (Vert si proche du centre, rouge sinon)
    uint16_t couleurBulle = ST77XX_RED;
    if (abs(angles.pitch) < 0.2 && abs(angles.roll) < 0.2)
        couleurBulle = ST77XX_GREEN;

    monEcran.fillCircle(newX, newY, 10, couleurBulle);

    // Sauvegarde pour le prochain tour
    oldX = newX;
    oldY = newY;
}

void MonEcran::afficherPrecision(uint8_t precision)
{
    // Affichage de la précision
    // Affichage graphique simple de la précision (vert = 3, cyan = 2, rouge = 0 ou 1)
    uint16_t couleurPrecision = ST77XX_RED;
    switch (precision)
    {
    case 3:
        couleurPrecision = ST77XX_GREEN;
        break;
    case 2:
        couleurPrecision = ST77XX_CYAN;
        break;
    default:
        couleurPrecision = ST77XX_RED;
    }
    monEcran.setTextColor(couleurPrecision, ST77XX_BLACK);
    monEcran.setCursor(0, 32);
    monEcran.printf("PREC:%d", precision);
}

void MonEcran::afficherAngles(EulerAngles angles)
{
    // Affichage numérique des angles
    monEcran.setCursor(0, 260);
    monEcran.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    monEcran.printf("Yaw:   %7.2f\n\r", angles.yaw);
    monEcran.printf("Pitch: %7.2f\n\r", angles.pitch);
    monEcran.printf("Roll:  %7.2f\n\r", angles.roll);
}
    
void MonEcran::loop(GPSManager *gpsManager)
{
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 10000)
        return;
    lastDisplay = millis();
    // affichage Batterie
    monEcran.setCursor(120, 48);
    monEcran.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    monEcran.printf("BAT:%.2fV", Batterie::lireTension());
    // affichage GPS
    monEcran.setCursor(0, 0);
    monEcran.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    monEcran.printf("UT: %s Sat:%d\n\r", gpsManager->timeString().c_str(), gpsManager->satellites());
    monEcran.printf("%05.2fN   %05.2fE\n\r", gpsManager->latitude(), gpsManager->longitude());
    // affichage angles
    EulerAngles angles = Telescope::getCurrentAngles();
    afficherAngles(angles);
    // affichage Niveau à bulle
    afficherNiveauBulle(angles);
    // affichage boussole
    afficherBoussole(angles);
    // affichage précision
    afficherPrecision(Telescope::getPrecision());
}

void MonEcran::printf(const char *format, ...)
{
    char buffer[256]; // Buffer pour stocker la chaîne formatée
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args); // Formate la chaîne
    va_end(args);
    monEcran.print(buffer); // Affiche la chaîne formatée sur l'écran
}
