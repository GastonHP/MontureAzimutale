#include "Telescope.hpp"

#include "Batterie.hpp"
#include "WebServer.hpp"
#include "monEcran.hpp"
#include "astronomy.h"
#include "gpsManager.hpp"
#include "log.hpp"

#include <Wire.h>

#define BNO_PIN_SDA 8
#define BNO_PIN_SCL 9
#define BNO_PIN_INT 13
#define BNO_PIN_RST_UND -1 // 21
#define BNO_I2CADDR_DEFAULT 0x4B

sh2_SensorValue_t Telescope::sensorValue;

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
#define PIN_SLEEP 39

bool Telescope::bno085_initialized = false;

MotorControl Telescope::motorAZ(PIN_AZ_STEP, PIN_AZ_DIR, PIN_M0, PIN_M1, PIN_M2, PIN_EN, PIN_SLEEP);
MotorControl Telescope::motorALT(PIN_ALT_STEP, PIN_ALT_DIR, PIN_M0, PIN_M1, PIN_M2, PIN_EN, PIN_SLEEP);

EulerAngles Telescope::ARVR_STABILIZED_RV_anglesActuels = EulerAngles(0.0f, 0.0f, 0.0f);
EulerAngles Telescope::ROTATION_VECTOR_anglesActuels = EulerAngles(0.0f, 0.0f, 0.0f);
EulerAngles Telescope::GAME_ROTATION_VECTOR_anglesActuels = EulerAngles(0.0f, 0.0f, 0.0f);

Calibration Telescope::ARVR_STABILIZED_RV_calibration;
Calibration Telescope::ROTATION_VECTOR_calibration;
Calibration Telescope::GAME_ROTATION_VECTOR_calibration;

Adafruit_BNO08x Telescope::bno08x = Adafruit_BNO08x(BNO_PIN_RST_UND); // Utilisation du même reset pin que dans bno08x.cpp
EulerAngles Telescope::anglesAAtteindre = EulerAngles(0.0f, 0.0f, 0.0f);

float Telescope::tolerance = 0.5; // Tolérance par défaut en degrés
float Telescope::maxSpeed = 1.0;  // Vitesse maximale par défaut
bool Telescope::setupOK = false;
bool Telescope::automatique = false;
bool Telescope::loopActif = false;

int Telescope::nbCommandes = 0;
int Telescope::commandes[Telescope::maxCommandes] = {0}; // Initialisation du tableau de commandes
long Telescope::parms1[Telescope::maxCommandes] = {0};   // Initialisation du tableau de paramètres 1
long Telescope::parms2[Telescope::maxCommandes] = {0};   // Initialisation du tableau de paramètres 2

// Variable volatile pour indiquer qu'une donnée est dispo (utilisée par l'interruption)
static volatile bool bnoDataAvailable = false;

static void logGen(String s, int niveau = 0)
{
    Serial.println("==>" + s);
    Log::addLog(s);
    MonEcran::log(s, niveau);
}

// Fonction ultra-courte appelée en arrière-plan dès que la broche INT passe à l'état BAS
static void IRAM_ATTR bnoISR() { bnoDataAvailable = true; }

#define interval_us 10000 // Intervalle de 10 ms pour les rapports de capteurs
void Telescope::setReports()
{
    if (bno085_initialized == false)
    {
        log("Impossible de configurer les rapports de capteurs : BNO085 non initialisé");
        return;
    }
    // Ici, vous pouvez activer les rapports de capteurs que vous souhaitez recevoir
    // Par exemple, pour obtenir les angles d'Euler stabilisés pour la réalité augmentée :
    // Initialiser le BNO08x et configurer les rapports de capteurs nécessaires

    if (!bno08x.enableReport(SH2_ARVR_STABILIZED_RV, interval_us)) // Rapport de rotation vector AR/VR stabilisé à 20 ms (50 Hz)
    {
        Serial.println("Could not enable AR/VR stabilized rotation vector");
        log("Erreur lors de l'activation du rapport AR/VR stabilized rotation vector");
    }
    else
    {
        log("Rapport AR/VR stabilized rotation vector activé avec succès");
    }
    if (!bno08x.enableReport(SH2_ROTATION_VECTOR, interval_us)) // Rapport de rotation vector AR/VR stabilisé à 20 ms (50 Hz)
    {
        Serial.println("Could not enable rotation vector");
        log("Erreur lors de l'activation du rapport rotation vector");
    }
    else
    {
        log("Rapport AR/VR stabilized rotation vector activé avec succès");
    }
    if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR, interval_us)) // Rapport de rotation vector AR/VR stabilisé à 20 ms (50 Hz)
    {
        Serial.println("Could not enable game rotation vector");
        log("Erreur lors de l'activation du rapport game rotation vector");
    }
    else
    {
        log("Rapport AR/VR stabilized rotation vector activé avec succès");
    }
}

void Telescope::setup()
{
    setAutomatique(false); // Par défaut, on démarre en mode manuel
    // Initialisation des moteurs pas à pas
    motorAZ.begin();
    motorALT.begin();
    motorAZ.setMicrostepping(32);  // Mode 1/16 pour plus de précision
    motorALT.setMicrostepping(32); // Mode 1/16 pour plus de précision
    log("Moteurs initialisés en mode 1/32 step");

    // // Initialisation du BNO08x
    // pinMode(BNO_PIN_RST, OUTPUT);
    // digitalWrite(BNO_PIN_RST, LOW);
    // // Attendre que le capteur soit bien réinitialisé
    // delay(20);
    // digitalWrite(BNO_PIN_RST, HIGH);
    // // Attendre que le BNO08x soit prêt après le reset
    // delay(100);
    // Initialisation du bus I2C à 400 kHz
    Wire.begin(BNO_PIN_SDA, BNO_PIN_SCL, 100000); // 100 kHz pour le BNO08x
    delay(200); // Attendre que le bus I2C soit stable
    if (!bno08x.begin_I2C(BNO_I2CADDR_DEFAULT, &Wire))
    {
        logGen("Erreur de connexion au BNO08x", 2);
        loopActif = true;
        return;
    }
    logGen("BNO08x connecté avec succès");
    bno085_initialized = true;
    // Configuration de l'interruption matérielle sur l'ESP32-S3
    // On détecte le passage du niveau HAUT au niveau BAS (FALLING)
    pinMode(BNO_PIN_INT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BNO_PIN_INT), bnoISR, FALLING);

    setupOK = true;
    logGen("Systeme Pret");
    // Affichage des informations sur les produits détectés
    for (int n = 0; n < bno08x.prodIds.numEntries; n++)
    {
        String s = "Part " + String(bno08x.prodIds.entry[n].swPartNumber, HEX) +
                   ": Version :" + String(bno08x.prodIds.entry[n].swVersionMajor, HEX) + "." + String(bno08x.prodIds.entry[n].swVersionMinor, HEX) +
                   "." + String(bno08x.prodIds.entry[n].swVersionPatch, HEX) +
                   " Build " + String(bno08x.prodIds.entry[n].swBuildNumber, HEX);
        logGen(s);
    }
    setReports();
    // addCommande(CMD_CalibrateMouvement); // Ajouter la commande de calibrage du mouvement à la liste des commandes à exécuter
    loopActif = true;
}
#define stepsForCalibration 200
#define Time2Wait 3000
void Telescope::calibrateAZ()
{
    log("Démarrage de la calibration de l'axe AZ...");
    if (!bno085_initialized)
    {
        log("Impossible de calibrer l'axe AZ : BNO085 non initialisé");
        return;
    }
    // Ici, vous pouvez implémenter une procédure de calibration spécifique pour l'axe AZ
    // Par exemple, vous pourriez faire tourner le moteur AZ d'une certaine quantité et mesurer la réponse du capteur pour calculer un coefficient de conversion spécifique à l'axe AZ
    // Cette fonction peut être appelée indépendamment pour recalibrer uniquement l'axe AZ si nécessaire
    // calibrer AZ
    readAnglesFromSensor(true); // Forcer la lecture des angles pour s'assurer d'avoir des données fraîches
    ARVR_STABILIZED_RV_calibration.origineAZ = ARVR_STABILIZED_RV_anglesActuels.copie();
    ROTATION_VECTOR_calibration.origineAZ = ROTATION_VECTOR_anglesActuels.copie();
    GAME_ROTATION_VECTOR_calibration.origineAZ = GAME_ROTATION_VECTOR_anglesActuels.copie();
    Telescope::steps(stepsForCalibration, 0); // Test de mouvement sur l'axe AZ
    while (isMoving())
        delay(100);
    delay(Time2Wait); // Attendre que les vibrations se calment

    readAnglesFromSensor(true); // Forcer la lecture des angles pour s'assurer d'avoir des données fraîches après le mouvement
    ARVR_STABILIZED_RV_calibration.finAZ = ARVR_STABILIZED_RV_anglesActuels.copie();
    ROTATION_VECTOR_calibration.finAZ = ROTATION_VECTOR_anglesActuels.copie();
    GAME_ROTATION_VECTOR_calibration.finAZ = GAME_ROTATION_VECTOR_anglesActuels.copie();

    Telescope::steps(-stepsForCalibration, 0);
}

void Telescope::calibrateALT()
{
    log("Démarrage de la calibration de l'axe ALT...");
    if (!bno085_initialized)
    {
        log("Impossible de calibrer l'axe ALT : BNO085 non initialisé");
        return;
    }
    // Ici, vous pouvez implémenter une procédure de calibration spécifique pour l'axe ALT
    // Par exemple, vous pourriez faire tourner le moteur ALT d'une certaine quantité et mesurer la réponse du capteur pour calculer un coefficient de conversion spécifique à l'axe ALT
    // Cette fonction peut être appelée indépendamment pour recalibrer uniquement l'axe ALT si nécessaire
    // Calibrer ALT
    ARVR_STABILIZED_RV_calibration.origineALT = ARVR_STABILIZED_RV_anglesActuels.copie();
    ROTATION_VECTOR_calibration.origineALT = ROTATION_VECTOR_anglesActuels.copie();
    GAME_ROTATION_VECTOR_calibration.origineALT = GAME_ROTATION_VECTOR_anglesActuels.copie();
    Telescope::steps(0, stepsForCalibration); // Test de mouvement sur l'axe ALT
    while (isMoving())
        delay(100);
    delay(Time2Wait); // Attendre que les vibrations se calment

    readAnglesFromSensor(true); // Forcer la lecture des angles pour s'assurer d'avoir des données fraîches après le mouvement
    ARVR_STABILIZED_RV_calibration.finALT = ARVR_STABILIZED_RV_anglesActuels.copie();
    ROTATION_VECTOR_calibration.finALT = ROTATION_VECTOR_anglesActuels.copie();
    GAME_ROTATION_VECTOR_calibration.finALT = GAME_ROTATION_VECTOR_anglesActuels.copie();

    Telescope::steps(0, -stepsForCalibration);
}

void Telescope::calibrateMovement()
{
    if (!bno085_initialized)
    {
        log("Impossible de calibrer le mouvement : BNO085 non initialisé");
        return;
    }
    calibrateAZ();
    while (isMoving())
        delay(100);
    delay(Time2Wait); // Attendre que les vibrations se calment

    calibrateALT();
    log("Calibration terminée. Coefficients de conversion calculés :");
    // log("Delta AZ par step : Yaw " + String(arvr_deltaAZ.yaw, 6) + "°, Pitch " + String(arvr_deltaAZ.pitch, 6) + "°, Roll " + String(arvr_deltaAZ.roll, 6) + "°");
    // log("Delta ALT par step : Yaw " + String(arvr_deltaALT.yaw, 6) + "°, Pitch " + String(arvr_deltaALT.pitch, 6) + "°, Roll " + String(arvr_deltaALT.roll, 6) + "°");
    // WebServer::setActivated(true); // Réactiver le serveur Web après la calibration
    // MonEcran::setActivated(true);  // Réactiver l'écran après la calibration
    // loopActif = true;
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

#define FrequenceDeBoucle 5 // 10 Hz
static unsigned long nextLoop = 0;
static double lastRoll = -9999.0;

void Telescope::loop()
{
    if (millis() < nextLoop)
        return;
    nextLoop = millis() + 1000 / FrequenceDeBoucle;
    if (!setupOK || !loopActif)
        return;
    if (nbCommandes > 0)
    {
        switch (commandes[0])
        {
        case CMD_CalibrateMouvement:
            log("Exécution de la commande : Calibrage du mouvement");
            calibrateMovement();
            break;
        case CMD_Steps:
            log("Exécution de la commande : Mouvement de " + String(parms1[0]) + " pas AZ et " + String(parms2[0]) + " pas ALT");
            steps(parms1[0], parms2[0]);
            break;
        case CMD_CalibrateAZ:
            log("Exécution de la commande : Calibrage de l'axe AZ");
            calibrateAZ();
            break;
        case CMD_CalibrateALT:
            log("Exécution de la commande : Calibrage de l'axe ALT");
            calibrateALT();
            break;
        default:
            log("Commande inconnue : " + String(commandes[0]));
            break;
        }
        // Décaler les commandes restantes
        for (int i = 1; i < nbCommandes; i++)
        {
            commandes[i - 1] = commandes[i];
            // Décaler les paramètres restants
            parms1[i - 1] = parms1[i];
            parms2[i - 1] = parms2[i];
        }
        nbCommandes--;
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
    observer.latitude = GPSManager::altitude();
    observer.longitude = GPSManager::longitude();
    observer.height = GPSManager::altitude();

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
    // // Ne pas lancer une nouvelle commande si les moteurs sont déjà en mouvement
    // if (isMoving())
    //     return;

    // EulerAngles aa = getCurrentAngles(true).copie();

    // // 3. Conversion en pas (Ratio AZ-Pronto ~120:1)
    // // 21333 pas = 360° / (120 * 200 * 32) -> environ 2133 pas par degré
    // if (arvr_deltaALT.roll != 0)
    // {
    //     // 2. Calculer l'erreur (différence)
    //     float erreurAlt = cibleAlt - aa.roll;
    //     if (erreurAlt > 180)
    //         erreurAlt -= 360;
    //     if (erreurAlt < -180)
    //         erreurAlt += 360;
    //     long pasALT = erreurAlt / arvr_deltaALT.roll; // Ajustement en fonction de la calibration
    //     motorALT.moveSteps(pasALT);
    // }
    // if (arvr_deltaAZ.yaw != 0)
    // {
    //     // 2. Calculer l'erreur (différence)
    //     float erreurAz = cibleAz - aa.yaw;
    //     // Gestion du passage 360/0 pour l'Azimut
    //     if (erreurAz > 180)
    //         erreurAz -= 360;
    //     if (erreurAz < -180)
    //         erreurAz += 360;
    //     long pasAZ = erreurAz / arvr_deltaAZ.yaw; // Ajustement en fonction de la calibration
    //     motorAZ.moveSteps(pasAZ);
    // }
}

void Telescope::setAutomatique(bool autoMode) { automatique = autoMode; }

void Telescope::readAnglesFromSensor(bool forceUpdate)
{
    if (bno085_initialized == false)
    {
        log("Lecture des angles depuis le capteur impossible : BNO085 non initialisé");
        return;
    }
    if (bno08x.wasReset())
    {
        Serial.print("sensor was reset ");
        setReports();
    }
    Telescope::log("Lecture des angles depuis le capteur...");
    bool hasNewData = bno08x.getSensorEvent(&sensorValue);
    if (!hasNewData && !forceUpdate)
    {
        return; // Pas de nouvelle donnée et pas de forçage, on garde les angles actuels
    }
    bool RV_received = false;
    bool ARVR_received = false;
    bool GRV_received = false;

    unsigned long startTime = millis();
    while ((RV_received == false || ARVR_received == false || GRV_received == false) && millis() - startTime < 15000) // Attendre jusqu'à 15000 ms pour obtenir de nouvelles données
    {
        if (hasNewData)
        {
            // Lire les angles actuels depuis le BNO08x
            switch (sensorValue.sensorId)
            {
            case SH2_ARVR_STABILIZED_RV:
                ARVR_STABILIZED_RV_anglesActuels = EulerAngles::getEulerFromQuaternion(
                    sensorValue.un.arvrStabilizedRV.i,
                    sensorValue.un.arvrStabilizedRV.j,
                    sensorValue.un.arvrStabilizedRV.k,
                    sensorValue.un.arvrStabilizedRV.real);
                // À ajouter dans votre fonction afficherInterface
                ARVR_STABILIZED_RV_anglesActuels.bno_timestamp = sensorValue.timestamp;
                ARVR_STABILIZED_RV_anglesActuels.esp_timestamp = millis();
                ARVR_STABILIZED_RV_anglesActuels.sensorId = sensorValue.sensorId;
                ARVR_STABILIZED_RV_anglesActuels.accuracy = sensorValue.un.arvrStabilizedRV.accuracy * 57.2958; // précision en degrés
                ARVR_STABILIZED_RV_anglesActuels.precision = sensorValue.status & 0x03;
                ARVR_received = true;
                break;
            case SH2_ROTATION_VECTOR:
                ROTATION_VECTOR_anglesActuels = EulerAngles::getEulerFromQuaternion(
                    sensorValue.un.rotationVector.i,
                    sensorValue.un.rotationVector.j,
                    sensorValue.un.rotationVector.k,
                    sensorValue.un.rotationVector.real);
                ROTATION_VECTOR_anglesActuels.bno_timestamp = sensorValue.timestamp;
                ROTATION_VECTOR_anglesActuels.esp_timestamp = millis();
                ROTATION_VECTOR_anglesActuels.sensorId = sensorValue.sensorId;
                ROTATION_VECTOR_anglesActuels.accuracy = sensorValue.un.rotationVector.accuracy * 57.2958; // précision en degrés
                ROTATION_VECTOR_anglesActuels.precision = sensorValue.status & 0x03;
                RV_received = true;
                break;
            case SH2_GAME_ROTATION_VECTOR:
                GAME_ROTATION_VECTOR_anglesActuels = EulerAngles::getEulerFromQuaternion(
                    sensorValue.un.gameRotationVector.i,
                    sensorValue.un.gameRotationVector.j,
                    sensorValue.un.gameRotationVector.k,
                    sensorValue.un.gameRotationVector.real);
                GAME_ROTATION_VECTOR_anglesActuels.bno_timestamp = sensorValue.timestamp;
                GAME_ROTATION_VECTOR_anglesActuels.esp_timestamp = millis();
                GAME_ROTATION_VECTOR_anglesActuels.sensorId = sensorValue.sensorId;
                GAME_ROTATION_VECTOR_anglesActuels.accuracy = -1;
                GAME_ROTATION_VECTOR_anglesActuels.precision = sensorValue.status & 0x03;
                GRV_received = true;
                break;
            default:
                break;
            }
        }
        hasNewData = bno08x.getSensorEvent(&sensorValue);
    }
    Telescope::log("Fin de la lecture des angles depuis le capteur en " + String(millis() - startTime) + " ms...");
    return;
}

void Telescope::steps(long stepsAz, long stepsAlt)
{
    while (isMoving())
    {
        // Attendre que les moteurs soient prêts pour une nouvelle commande
        delay(100);
    }
    if (stepsAz != 0)
        motorAZ.moveSteps(stepsAz);
    if (stepsAlt != 0)
        motorALT.moveSteps(stepsAlt);
}

void Telescope::stop()
{
    motorAZ.moveSteps(0);
    motorALT.moveSteps(0);
}

void Telescope::calibrate()
{
    unsigned long timeToWait = 1000; // 1 seconde
    setMicrostepping(32);            // Mode 1/8 pour un calibrage plus rapide
    for (int i = 0; i < 1; i++)
    {
        Telescope::steps(0, 300); // Mouvement rapide pour sortir d'éventuels obstacles
        while (isMoving())
            delay(100);
        Telescope::steps(0, -300); // Retour à la position initiale
        while (isMoving())
            delay(100);
        Telescope::steps(300, 0); // Mouvement rapide pour sortir d'éventuels obstacles
        while (isMoving())
            delay(100);
        Telescope::steps(-300, 0); // Retour à la position initiale
        while (isMoving())
            delay(100);
    }

    // Telescope::stop();
    // EulerAngles angles = getCurrentAngles(true).copie();
    // Telescope::steps(100, 0); // Test de mouvement sur l'axe AZ
    // delay(2000);              // Attendre que le mouvement soit terminé
    // EulerAngles newAngles = getCurrentAngles(true).copie();
    // deltaAZ = EulerAngles(
    //     newAngles.yaw - angles.yaw,
    //     newAngles.pitch - angles.pitch,
    //     newAngles.roll - angles.roll);
    // angles = getCurrentAngles(true).copie();
    // Telescope::steps(0, 100); // Test de mouvement sur l'axe ALT
    // delay(2000);              // Attendre que le mouvement soit terminé
    // newAngles = getCurrentAngles(true).copie();
    // deltaALT = EulerAngles(
    //     newAngles.yaw - angles.yaw,     // À ajuster en fonction de l'orientation du capteur
    //     newAngles.pitch - angles.pitch, // À ajuster en fonction de l'orientation du capteur
    //     newAngles.roll - angles.roll);  // À ajuster en fonction de l'orientation du capteur
    // Telescope::steps(-100, 0);
    // Telescope::steps(0, -100);
}

void Telescope::setMicrostepping(int ms)
{
    motorAZ.setMicrostepping(ms);
    motorALT.setMicrostepping(ms);
}

String Telescope::logBuffer[Telescope::maxLogLines] = {""};

void Telescope::log(String s)
{
    // Décaler les anciennes lignes vers le haut du buffer
    for (int i = 0; i < maxLogLines - 1; i++)
    {
        logBuffer[i] = logBuffer[i + 1];
    }
    // Ajouter la nouvelle ligne en haut
    logBuffer[maxLogLines - 1] = s;
}
static uint64_t startTime = 0;
String bnoTimestampToString(uint64_t ts_us, uint64_t esp_timestamp_us)
{
    if (startTime == 0)
    {
        startTime = ts_us; // Initialiser le temps de départ à la première lecture
    }
    float seconds = (ts_us - startTime) / 1e6; // µs → secondes

    char buf[40];
    snprintf(buf, sizeof(buf), "%d µs /%d ms", ts_us, esp_timestamp_us);

    return String(buf);
}

String Telescope::getJson()
{
    String json = "{";
    json += "\"arvr_yaw\":" + String(ARVR_STABILIZED_RV_anglesActuels.yaw, 2) + ",";
    json += "\"arvr_pitch\":" + String(ARVR_STABILIZED_RV_anglesActuels.pitch, 2) + ",";
    json += "\"arvr_roll\":" + String(ARVR_STABILIZED_RV_anglesActuels.roll, 2) + ",";
    json += "\"arvr_accuracy\":" + String(ARVR_STABILIZED_RV_anglesActuels.accuracy, 2) + ",";
    json += "\"arvr_precision\":" + String(ARVR_STABILIZED_RV_anglesActuels.precision) + ",";
    json += "\"arvr_timestamp\":\"" + bnoTimestampToString(ARVR_STABILIZED_RV_anglesActuels.bno_timestamp, ARVR_STABILIZED_RV_anglesActuels.esp_timestamp) + "\",";
    json += "\"arvr_deltaALT\":\"" + Telescope::ARVR_STABILIZED_RV_calibration.deltaALT().toString() + "\",";
    json += "\"arvr_deltaAZ\":\"" + Telescope::ARVR_STABILIZED_RV_calibration.deltaAZ().toString() + "\",";

    json += "\"rotation_vector_yaw\":" + String(ROTATION_VECTOR_anglesActuels.yaw, 2) + ",";
    json += "\"rotation_vector_pitch\":" + String(ROTATION_VECTOR_anglesActuels.pitch, 2) + ",";
    json += "\"rotation_vector_roll\":" + String(ROTATION_VECTOR_anglesActuels.roll, 2) + ",";
    json += "\"rotation_vector_accuracy\":" + String(ROTATION_VECTOR_anglesActuels.accuracy, 2) + ",";
    json += "\"rotation_vector_precision\":" + String(ROTATION_VECTOR_anglesActuels.precision) + ",";
    json += "\"rotation_vector_timestamp\":\"" + bnoTimestampToString(ROTATION_VECTOR_anglesActuels.bno_timestamp, ROTATION_VECTOR_anglesActuels.esp_timestamp) + "\",";
    json += "\"rotation_vector_deltaALT\":\"" + Telescope::ROTATION_VECTOR_calibration.deltaALT().toString() + "\",";
    json += "\"rotation_vector_deltaAZ\":\"" + Telescope::ROTATION_VECTOR_calibration.deltaAZ().toString() + "\",";

    json += "\"game_rotation_vector_yaw\":" + String(GAME_ROTATION_VECTOR_anglesActuels.yaw, 2) + ",";
    json += "\"game_rotation_vector_pitch\":" + String(GAME_ROTATION_VECTOR_anglesActuels.pitch, 2) + ",";
    json += "\"game_rotation_vector_roll\":" + String(GAME_ROTATION_VECTOR_anglesActuels.roll, 2) + ",";
    json += "\"game_rotation_vector_accuracy\":" + String(GAME_ROTATION_VECTOR_anglesActuels.accuracy, 2) + ",";
    json += "\"game_rotation_vector_precision\":" + String(GAME_ROTATION_VECTOR_anglesActuels.precision) + ",";
    json += "\"game_rotation_vector_timestamp\":\"" + bnoTimestampToString(GAME_ROTATION_VECTOR_anglesActuels.bno_timestamp, GAME_ROTATION_VECTOR_anglesActuels.esp_timestamp) + "\",";
    json += "\"game_rotation_vector_deltaALT\":\"" + Telescope::GAME_ROTATION_VECTOR_calibration.deltaALT().toString() + "\",";
    json += "\"game_rotation_vector_deltaAZ\":\"" + Telescope::GAME_ROTATION_VECTOR_calibration.deltaAZ().toString() + "\",";

    json += "\"batt\":\"" + String(Batterie::lireTension(), 1) + "\",";
    json += "\"sats\":\"" + String(GPSManager::satellites()) + "\",";

    String logConcatene = "";
    for (int i = Telescope::maxLogLines - 1; i >= 0; i--)
    {
        logConcatene += Telescope::logBuffer[i] + "<br>";
    }
    json += "\"log\":\"" + logConcatene + "\",";
    json += "\"EOJ\":\"---\"}";
    return json;
}