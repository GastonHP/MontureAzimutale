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
#define TFT_ROTATION 1

// définition pour les displays TFT

#define LH 20 // Hauteur d'une ligne de texte
#define ANGX 0
#define ANGY 0
#define ANGW 160
#define ANGH (LH * 4 + 2)

#define BATX 0
#define BATY (ANGX + ANGH + 2)
#define BATW ANGW
#define BATH (LH + 2)

#define GPSX 0
#define GPSY (BATY + BATH + 2)
#define GPSW ANGW
#define GPSH (LH * 4 + 2)

static Adafruit_ST7789 monEcran = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
bool MonEcran::activated = true;

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

    int cX = 240;  // Centre X
    int cY = 180; // Centre Y
    int r = 60;   // Rayon du cadran

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
    static int oldX = 240;
    static int oldY = 180;
    int centreX = 240;
    int centreY = 60;
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
        monEcran.fillCircle(oldX, oldY, 10, ST77XX_BLACK);
 
    // 5. Dessiner la nouvelle bulle (Vert si proche du centre, rouge sinon)
    uint16_t couleurBulle = ST77XX_RED;
    if (abs(angles.pitch) < 0.2 && abs(angles.roll) < 0.2)
        couleurBulle = ST77XX_GREEN;

    monEcran.fillCircle(newX, newY, 10, couleurBulle);

    // Sauvegarde pour le prochain tour
    oldX = newX;
    oldY = newY;
}

void MonEcran::afficherAngles(EulerAngles angles)
{
    // Affichage numérique des angles
    // rectangle de 170x50 pixels en bas à gauche

    monEcran.drawRect(ANGX, ANGY, ANGW, ANGH, ST77XX_GREEN);
    monEcran.setCursor(ANGX + 2, ANGY + 2);
    monEcran.setTextSize(2);
    monEcran.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    monEcran.printf("Yaw:  %7.2f\n\r", angles.yaw);
    monEcran.setCursor(ANGX + 2, ANGY + LH);
    monEcran.printf("Pitch:%7.2f\n\r", angles.pitch);
    monEcran.setCursor(ANGX + 2, ANGY + 2 * LH);
    monEcran.printf("Roll: %7.2f\n\r", angles.roll);
    monEcran.setCursor(ANGX + 2, ANGY + 3 * LH);
    uint16_t couleurPrecision = ST77XX_RED;
    switch (angles.precision)
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
    monEcran.printf("PREC:%7d", angles.precision);
}

void MonEcran::afficherBatterie()
{
    // Affichage de la tension de la batterie

    monEcran.drawRect(BATX, BATY, BATW, BATH, ST77XX_GREEN);
    monEcran.setCursor(BATX + 2, BATY + 2);
    monEcran.setTextSize(2);
    monEcran.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    monEcran.printf("BAT: %7.2fV", Batterie::lireTension());
}

void MonEcran::afficherGPS()
{
    // Affichage des données GPS
    monEcran.drawRect(GPSX, GPSY, GPSW, GPSH, ST77XX_GREEN);
    monEcran.setCursor(GPSX + 2, GPSY + 2);
    monEcran.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
    monEcran.printf("UT: %s\n\r",GPSManager::timeString().c_str());
    monEcran.setCursor(GPSX + 2, GPSY + LH);
    monEcran.printf("Sat:%7d\n\r", GPSManager::satellites());
    monEcran.setCursor(GPSX + 2, GPSY + 2 * LH);
    monEcran.printf("Lat:%7.2fN\n\r", GPSManager::latitude());
    monEcran.setCursor(GPSX + 2, GPSY + 3 * LH);
    monEcran.printf("Lon:%7.2fE\n\r", GPSManager::longitude());
}

void MonEcran::afficherQuadrilage(int step)
{
    // Affichage d'un quadrillage pour aider à l'alignement
    int spacing = 20; // Espacement entre les lignes
    for (int x = 0; x < monEcran.width(); x += spacing)
    {
        monEcran.drawLine(x, 0, x, monEcran.height(), ST77XX_CYAN);
    }
    for (int y = 0; y < monEcran.height(); y += spacing)
    {
        monEcran.drawLine(0, y, monEcran.width(), y, ST77XX_CYAN);
    }
}
void MonEcran::loop()
{
    if(!activated)
        return;
    static unsigned long lastDisplay = 0;
    static bool firstLoop = true;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
    if (firstLoop)
    {
        monEcran.fillScreen(ST77XX_BLACK);
        firstLoop = false;
    }
    // affichage angles
    Telescope::readAnglesFromSensor();
    EulerAngles angles = Telescope::ARVR_STABILIZED_RV_anglesActuels;
    afficherAngles(angles);
    // affichage Batterie
    afficherBatterie();
    // affichage GPS
    afficherGPS();
    // affichage Niveau à bulle
     afficherNiveauBulle(angles);
    // affichage boussole
    afficherBoussole(angles);
    // affichage quadrillage
    // afficherQuadrilage(10);
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
