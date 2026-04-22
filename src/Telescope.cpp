#include "Telescope.hpp"
#include <Wire.h>

#define sda_pin 8
#define scl_pin 9

static sh2_SensorValue_t sensorValue;

// Instanciation globale
// Azimut : Step 5, Dir 6 | Altitude : Step 7, Dir 15
// M0=2, M1=42, M2=41 (partagés pour les deux drivers)

#define PIN_AZ_STEP 5
#define PIN_AZ_DIR 6
#define PIN_ALT_STEP 7
#define PIN_ALT_DIR 15

#define PIN_M0 2
#define PIN_M1 42
#define PIN_M2 41

#define PIN_EN 16
#define PIN_SLEEP 38

MotorControl Telescope::motorAZ(PIN_AZ_STEP, PIN_AZ_DIR, PIN_M0, PIN_M1, PIN_M2, PIN_EN, PIN_SLEEP);
MotorControl Telescope::motorALT(PIN_ALT_STEP, PIN_ALT_DIR, PIN_M0, PIN_M1, PIN_M2, PIN_EN, PIN_SLEEP);
GPSManager *Telescope::gpsManager = nullptr;

EulerAngles Telescope::anglesActuels = EulerAngles(0.0f, 0.0f, 0.0f);
Adafruit_BNO08x Telescope::bno08x = Adafruit_BNO08x(-1); // Utilisation du même reset pin que dans bno08x.cpp
EulerAngles Telescope::anglesAAtteindre = EulerAngles(0.0f, 0.0f, 0.0f);
Adafruit_ST7789 *Telescope::tftptr = nullptr;
float Telescope::tolerance = 0.5; // Tolérance par défaut en degrés
float Telescope::maxSpeed = 1.0;  // Vitesse maximale par défaut
bool Telescope::setupOK = false;
bool Telescope::automatique = false;

void Telescope::setReports()
{
    // Ici, vous pouvez activer les rapports de capteurs que vous souhaitez recevoir
    // Par exemple, pour obtenir les angles d'Euler stabilisés pour la réalité augmentée :
    if (!bno08x.enableReport(SH2_ARVR_STABILIZED_RV))
    {
        Serial.println("Could not enable AR/VR stabilized rotation vector");
    }
}

void Telescope::setup(Adafruit_ST7789 *tftptr)
{
    Telescope::tftptr = tftptr;
    setAutomatique(false); // Par défaut, on démarre en mode manuel
    Wire.begin(sda_pin, scl_pin, 400000); // Initialisation du bus I2C à 400 kHz
    if (!bno08x.begin_I2C(0x4B, &Wire))
    {
        tftptr->setTextColor(ST77XX_RED);
        tftptr->setTextSize(2);
        tftptr->setCursor(10, 40);
        tftptr->print("Erreur BNO!");
        return;
    }
    setupOK = true;
    Serial.println("BNO08x connected!");
    tftptr->fillScreen(ST77XX_BLACK);
    tftptr->setTextColor(ST77XX_GREEN);
    tftptr->setTextSize(2);
    tftptr->setCursor(5, 5);
    tftptr->println("Systeme Pret");
    tftptr->setTextSize(1);
    for (int n = 0; n < bno08x.prodIds.numEntries; n++)
    {
        String s = "Part " + String(bno08x.prodIds.entry[n].swPartNumber, HEX) +
                   ": Version :" + String(bno08x.prodIds.entry[n].swVersionMajor, HEX) + "." + String(bno08x.prodIds.entry[n].swVersionMinor, HEX) +
                   "." + String(bno08x.prodIds.entry[n].swVersionPatch, HEX) +
                   " Build " + String(bno08x.prodIds.entry[n].swBuildNumber, HEX);
        Serial.println(s);
        tftptr->println(s);
    }
    // Initialiser le BNO08x et configurer les rapports de capteurs nécessaires
    if (!bno08x.enableReport(SH2_ARVR_STABILIZED_RV))
    {
        Serial.println("Could not enable AR/VR stabilized rotation vector");
    }
    motorAZ.begin();
    motorALT.begin();

    Serial.println("Moteurs prêts en mode 1/32 step.");
}

void Telescope::display_angles(EulerAngles angles)
{
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
    tftptr->setCursor(0, 260);
    tftptr->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tftptr->printf("Yaw:   %7.2f\n\r", angles.yaw);
    tftptr->printf("Pitch: %7.2f\n\r", angles.pitch);
    tftptr->printf("Roll:  %7.2f\n\r", angles.roll);
}

void Telescope::dessinerNiveauVif(EulerAngles angles)
{
    static unsigned long lastDisplay = 0;
    static int oldX = 80;
    static int oldY = 160;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
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
    tftptr->drawCircle(centreX, centreY, 50, ST77XX_WHITE);
    tftptr->drawCircle(centreX, centreY, 5, ST77XX_WHITE); // Point central

    // 4. Effacer l'ancienne bulle (en la dessinant en noir)
    if (newX != oldX || newY != oldY)
    {
        tftptr->fillCircle(oldX, oldY, 10, ST77XX_BLACK);
    }

    // 5. Dessiner la nouvelle bulle (Vert si proche du centre, rouge sinon)
    uint16_t couleurBulle = ST77XX_RED;
    if (abs(angles.pitch) < 0.2 && abs(angles.roll) < 0.2)
        couleurBulle = ST77XX_GREEN;

    tftptr->fillCircle(newX, newY, 10, couleurBulle);

    // Sauvegarde pour le prochain tour
    oldX = newX;
    oldY = newY;
}

void Telescope::dessinerBoussole(EulerAngles angles, bool vraiNord /*= true*/)
{
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
    float azimut = angles.yaw; // Azimut magnétique

    uint16_t couleurAiguille = ST77XX_BLUE;
    if (vraiNord)
    {
        azimut = calculerAzimutVrai(azimut);
        couleurAiguille = ST77XX_GREEN;
    }

    int cX = 80;  // Centre X
    int cY = 160; // Centre Y
    int r = 80;   // Rayon du cadran

    // 1. Dessiner le cadran fixe (une seule fois idéalement)
    tftptr->drawCircle(cX, cY, r, ST77XX_WHITE);
    tftptr->setTextColor(ST77XX_RED);
    tftptr->setCursor(cX - 5, cY - r - 15);
    tftptr->print("N");
    tftptr->setTextColor(ST77XX_WHITE);
    tftptr->setCursor(cX - 5, cY + r + 5);
    tftptr->print("S");
    // 2. Calculer l'angle de l'aiguille (en radians)
    float rad = azimut * PI / 180.0;

    // 3. Effacer l'ancienne aiguille (méthode simple : redessiner le fond)
    static float lastRad = 0;
    tftptr->drawLine(cX, cY, cX + sin(lastRad) * r, cY - cos(lastRad) * r, ST77XX_BLACK);

    // 4. Dessiner la nouvelle aiguille
    // Nord en Rouge
    tftptr->drawLine(cX, cY, cX + sin(rad) * r, cY - cos(rad) * r, couleurAiguille);
    // Sud en Blanc (optionnel)
    tftptr->drawLine(cX, cY, cX - sin(rad) * (r / 2), cY + cos(rad) * (r / 2), ST77XX_WHITE);

    lastRad = rad;

    // 5. Affichage numérique au centre
    tftptr->fillRect(cX - 20, cY + 20, 40, 15, ST77XX_BLACK); // Effacement partiel
    tftptr->setCursor(cX - 15, cY + 20);
    tftptr->printf("%03d", (int)azimut);
}

float Telescope::calculerAzimutVrai(float azimutMagnetique)
{
    // 1. Récupérer la longitude depuis le NEO-6M
    // float longitude = gps.location.lng();

    // 2. Formule simplifiée pour l'Europe de l'Ouest (approximation 2024-2026)
    // La déclinaison augmente d'environ 0.15° par an.
    // Pour Mouscron (Long: 3.2° E), la déclinaison est d'environ +2.8°
    float declinaison = 2.8;

    float azimutVrai = azimutMagnetique + declinaison;

    // 3. Rester dans la plage 0-360°
    if (azimutVrai >= 360)
        azimutVrai -= 360;
    if (azimutVrai < 0)
        azimutVrai += 360;

    return azimutVrai;
}

#define FrequenceDeBoucle 10 // 10 Hz
static unsigned long nextLoop = 0;
static double lastRoll = -9999.0;
void Telescope::loop()
{
    float accuracy;
    if (millis() < nextLoop)
        return;
    nextLoop = millis() + 1000 / FrequenceDeBoucle;
    if (!setupOK)
        return;
    if (bno08x.wasReset())
    {
        Serial.print("sensor was reset ");
        setReports();
    }
    if (!bno08x.getSensorEvent(&sensorValue))
    {
        return;
    }
    // On récupère le statut global de la précision
    // sensorValue.status contient la valeur (0 à 3)
    uint8_t precision = sensorValue.status & 0x03;

    // Affichage graphique simple de la précision (vert = 3, cyan = 2, rouge = 0 ou 1)
    switch (precision)
    {
    case 3:
        tftptr->setTextColor(ST77XX_GREEN, ST77XX_BLACK);
        break;
    case 2:
        tftptr->setTextColor(ST77XX_CYAN, ST77XX_BLACK);
        break;
    default:
        tftptr->setTextColor(ST77XX_RED, ST77XX_BLACK);
    }
    tftptr->setCursor(0, 32);
    tftptr->printf("ACC:%d", precision);
    // Lire les angles actuels depuis le BNO08x
    if (sensorValue.sensorId == SH2_ARVR_STABILIZED_RV)
    {
        anglesActuels = EulerAngles::getEulerFromQuaternion(
            sensorValue.un.arvrStabilizedRV.i,
            sensorValue.un.arvrStabilizedRV.j,
            sensorValue.un.arvrStabilizedRV.k,
            sensorValue.un.arvrStabilizedRV.real);

        // À ajouter dans votre fonction afficherInterface
        accuracy = sensorValue.un.arvrStabilizedRV.accuracy; // 0 = Invalide, 3 = Précision max
        tftptr->setCursor(72, 32);
        tftptr->printf("CAL: %f", accuracy);
        display_angles(anglesActuels);
        dessinerNiveauVif(anglesActuels);
        dessinerBoussole(anglesActuels, true); // true ==> azimuth vrai
        //commanderMouvement(anglesAAtteindre.yaw, anglesAAtteindre.pitch);
    }
}

void Telescope::setTarget(EulerAngles target, float tolerance, float maxSpeed)
{
    Telescope::anglesAAtteindre = target;
    Telescope::tolerance = tolerance;
    Telescope::maxSpeed = maxSpeed;
    commanderMouvement(target.yaw, target.pitch);
}

#define Astronomy_Observer astro_observer_t
#define Astronomy_Time astro_time_t
#define Astronomy_EquatorialCoords astro_equatorial_t
#define Astronomy_HorizontalCoordinates astro_horizon_t

void Telescope::pointer(float ascensionDroite, float declinaison)
{
    // 1. Définir l'observateur (GPS)
    Astronomy_Observer observer;
    observer.latitude = gpsManager->altitude();
    observer.longitude = gpsManager->longitude();
    observer.height = gpsManager->altitude();

    // 2. Temps actuel
    Astronomy_Time time = Astronomy_CurrentTime();

    // 3. Calculer la position locale
    // On crée un point céleste pour l'étoile
    Astronomy_EquatorialCoords star_eq;
    star_eq.ra = ascensionDroite;
    star_eq.dec = declinaison;
    // star_eq.dist = 642.0; // Distance en années-lumière (optionnel)

    // Transformation en Azimut/Altitude
    Astronomy_HorizontalCoordinates star_hor = Astronomy_Horizon(&time, observer, star_eq.ra, star_eq.dec, REFRACTION_NONE);

    Serial.print("Bételgeuse est à - Azimut: ");
    Serial.print(star_hor.azimuth);
    Serial.print(" | Altitude: ");
    Serial.println(star_hor.altitude);

    // 4. Lancer le mouvement Go-To
    // On utilise notre classe AsyncMotor avec rampe
    commanderMouvement(star_hor.azimuth, star_hor.altitude);
}

void Telescope::commanderMouvement(float cibleAz, float cibleAlt)
{
    // Ne pas lancer une nouvelle commande si les moteurs sont déjà en mouvement
    if(isMoving())
        return; 
    // 2. Calculer l'erreur (différence)
    float erreurAz = cibleAz - anglesActuels.yaw;
    float erreurAlt = cibleAlt - anglesActuels.roll;

    // Gestion du passage 360/0 pour l'Azimut
    if (erreurAz > 180)
        erreurAz -= 360;
    if (erreurAz < -180)
        erreurAz += 360;

    // 3. Conversion en pas (Ratio AZ-Pronto ~120:1)
    // 21333 pas = 360° / (120 * 200 * 32) -> environ 2133 pas par degré
    long pasAZ = erreurAz * 2133;
    long pasALT = erreurAlt * 2133;

    // 4. Lancer les moteurs (en tâche de fond avec les Timers)
    motorAZ.moveSteps(pasAZ);
    motorALT.moveSteps(pasALT);
}

void Telescope::setAutomatique(bool autoMode)
{
    automatique = autoMode;
}

EulerAngles Telescope::getCurrentAngles() { return anglesActuels; }

void Telescope::steps(long stepsAz, long stepsAlt)
{
    motorAZ.moveSteps(stepsAz);
    motorALT.moveSteps(stepsAlt);
}

void Telescope::stop()
{
    motorAZ.moveSteps(0);
    motorALT.moveSteps(0);
}