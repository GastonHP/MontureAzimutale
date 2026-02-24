#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

// For SPI mode, we need a CS pin
// #define BNO08X_CS 5
// #define BNO08X_INT 7

// For SPI mode, we also need a RESET
#define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// Variables pour mémoriser l'ancienne position (pour l'effaçage propre)
int oldX = 120;
int oldY = 160;

// À mettre dans ton setup() après bno08x.begin()
void activerCalibration()
{
    // On configure le "Dynamic Calibration"
    // magCalEnabled = 1, accelCalEnabled = 1, gyroCalEnabled = 1
    sh2_setCalConfig(SH2_CAL_MAG | SH2_CAL_ACCEL | SH2_CAL_GYRO);
    // sh2_saveConfig(); // Sauvegarde dans la mémoire non volatile du BNO08x
}

float calculerAzimutVrai(float azimutMagnetique)
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

void bno08x_dessinerBoussole(float azimut, Adafruit_ST7789 *tftptr, bool vraiNord = true)
{
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
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

void bno08x_dessinerNiveauVif(float pitch, float roll, Adafruit_ST7789 *tftptr)
{
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
    int centreX = 80;
    int centreY = 160;
    int sensibilite = 8; // Ajustez pour rendre la bulle plus ou moins vive

    // 1. Calcul de la nouvelle position (Inversion du signe selon l'orientation du capteur)
    int newX = centreX + (int)(roll * sensibilite);
    int newY = centreY + (int)(pitch * sensibilite);

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
    if (abs(pitch) < 0.2 && abs(roll) < 0.2)
        couleurBulle = ST77XX_GREEN;

    tftptr->fillCircle(newX, newY, 10, couleurBulle);

    // Sauvegarde pour le prochain tour
    oldX = newX;
    oldY = newY;
}
// Here is where you define the sensor outputs you want to receive
void setReports(void)
{
    Serial.println("Setting desired reports");
    // if (!bno08x.enableReport(SH2_ACCELEROMETER))
    // {
    //     Serial.println("Could not enable accelerometer");
    // }
    // if (!bno08x.enableReport(SH2_GYROSCOPE_CALIBRATED))
    // {
    //     Serial.println("Could not enable gyroscope");
    // }
    // if (!bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED))
    // {
    //     Serial.println("Could not enable magnetic field calibrated");
    // }
    // if (!bno08x.enableReport(SH2_LINEAR_ACCELERATION))
    // {
    //     Serial.println("Could not enable linear acceleration");
    // }
    // if (!bno08x.enableReport(SH2_GRAVITY))
    // {
    //     Serial.println("Could not enable gravity vector");
    // }
    if (!bno08x.enableReport(SH2_ROTATION_VECTOR))
    {
        Serial.println("Could not enable rotation vector");
    }
    // if (!bno08x.enableReport(SH2_GEOMAGNETIC_ROTATION_VECTOR))
    // {
    //     Serial.println("Could not enable geomagnetic rotation vector");
    // }
    // if (!bno08x.enableReport(SH2_ARVR_STABILIZED_RV))
    // {
    //     Serial.println("Could not enable AR/VR stabilized rotation vector");
    // }

    // if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR))
    // {
    //     Serial.println("Could not enable game rotation vector");
    // }
    // if (!bno08x.enableReport(SH2_STEP_COUNTER))
    // {
    //     Serial.println("Could not enable step counter");
    // }
    // if (!bno08x.enableReport(SH2_STABILITY_CLASSIFIER))
    // {
    //     Serial.println("Could not enable stability classifier");
    // }
    // if (!bno08x.enableReport(SH2_RAW_ACCELEROMETER))
    // {
    //     Serial.println("Could not enable raw accelerometer");
    // }
    // if (!bno08x.enableReport(SH2_RAW_GYROSCOPE))
    // {
    //     Serial.println("Could not enable raw gyroscope");
    // }
    // if (!bno08x.enableReport(SH2_RAW_MAGNETOMETER))
    // {
    //     Serial.println("Could not enable raw magnetometer");
    // }
    // if (!bno08x.enableReport(SH2_SHAKE_DETECTOR))
    // {
    //     Serial.println("Could not enable shake detector");
    // }
    // if (!bno08x.enableReport(SH2_PERSONAL_ACTIVITY_CLASSIFIER))
    // {
    //     Serial.println("Could not enable personal activity classifier");
    // }
}

void printActivity(uint8_t activity_id)
{
    switch (activity_id)
    {
    case PAC_UNKNOWN:
        Serial.print("Unknown");
        break;
    case PAC_IN_VEHICLE:
        Serial.print("In Vehicle");
        break;
    case PAC_ON_BICYCLE:
        Serial.print("On Bicycle");
        break;
    case PAC_ON_FOOT:
        Serial.print("On Foot");
        break;
    case PAC_STILL:
        Serial.print("Still");
        break;
    case PAC_TILTING:
        Serial.print("Tilting");
        break;
    case PAC_WALKING:
        Serial.print("Walking");
        break;
    case PAC_RUNNING:
        Serial.print("Running");
        break;
    case PAC_ON_STAIRS:
        Serial.print("On Stairs");
        break;
    default:
        Serial.print("NOT LISTED");
    }
    Serial.print(" (");
    Serial.print(activity_id);
    Serial.print(")");
}

void display_angles(float yaw, float pitch, float roll, Adafruit_ST7789 *tftptr)
{
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay < 3000)
        return;
    lastDisplay = millis();
    tftptr->setCursor(0, 260);
    tftptr->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tftptr->printf("Yaw:   %7.2f\n\r", yaw);
    tftptr->printf("Pitch: %7.2f\n\r", pitch);
    tftptr->printf("Roll:  %7.2f\n\r", roll);
}
static bool setupOK = false;

void bno08x_setup(Adafruit_ST7789 *tftptr)
{
    Wire.begin(8, 9);

    tftptr->setTextSize(2);
    tftptr->print("setup BNO!");
    // delay(1000);
    //  3. Initialiser le capteur BNO085
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

    setReports();

    Serial.println("Reading events");
    delay(1000);
    tftptr->fillScreen(ST77XX_BLACK);
    tftptr->setTextColor(ST77XX_GREEN);
    tftptr->setTextSize(2);
}

static float yaw, pitch, roll;
static float qw, qx, qy, qz;
static sh2_StabilityClassifier_t stability;
static sh2_ShakeDetector_t detection;
static sh2_PersonalActivityClassifier_t activity;
static double cap;
static unsigned long nextLoop = 0;

void bno08x_loop(Adafruit_ST7789 *tftptr)
{
    float accuracy;
    uint8_t calibrationStatus;
    if (millis() < nextLoop)
        return;
    nextLoop = millis() + 1000; // 20 Hz
    Serial.println("Loop BNO08x start:" + String(millis(), DEC));
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
    switch (sensorValue.sensorId)
    {
    case SH2_ARVR_STABILIZED_RV:
        Serial.print("AR/VR Stabilized Rotation Vector - r: ");
        Serial.print(sensorValue.un.arvrStabilizedRV.real);
        Serial.print(" i: ");
        Serial.print(sensorValue.un.arvrStabilizedRV.i);
        Serial.print(" j: ");
        Serial.print(sensorValue.un.arvrStabilizedRV.j);
        Serial.print(" k: ");
        Serial.println(sensorValue.un.arvrStabilizedRV.k);

        qw = sensorValue.un.arvrStabilizedRV.real;
        qx = sensorValue.un.arvrStabilizedRV.i;
        qy = sensorValue.un.arvrStabilizedRV.j;
        qz = sensorValue.un.arvrStabilizedRV.k;

        // Conversion quaternion → yaw/pitch/roll
        yaw = atan2f(2.0f * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz) * 57.2958f;
        pitch = asinf(2.0f * (qw * qy - qx * qz)) * 57.2958f;
        roll = atan2f(2.0f * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz) * 57.2958f;

        // À ajouter dans votre fonction afficherInterface
        accuracy = sensorValue.un.arvrStabilizedRV.accuracy; // 0 = Invalide, 3 = Précision max
        tftptr->setCursor(72, 32);
        tftptr->printf("CAL: %f", accuracy);
        display_angles(yaw, pitch, roll, tftptr);
        bno08x_dessinerNiveauVif(pitch, roll, tftptr);
        // bno08x_dessinerBoussole(yaw, tftptr);
        bno08x_dessinerBoussole(yaw, tftptr, true); // Magnétique
        break;
    case SH2_ACCELEROMETER:
        Serial.print("Accelerometer - x: ");
        Serial.print(sensorValue.un.accelerometer.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.accelerometer.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.accelerometer.z);
        break;
    case SH2_GYROSCOPE_CALIBRATED:
        Serial.print("Gyro - x: ");
        Serial.print(sensorValue.un.gyroscope.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.gyroscope.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.gyroscope.z);
        break;
    case SH2_MAGNETIC_FIELD_CALIBRATED:
        Serial.print("Magnetic Field - x: ");
        Serial.print(sensorValue.un.magneticField.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.magneticField.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.magneticField.z);
        tftptr->setCursor(0, 240);
        tftptr->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        tftptr->printf("MFC x:   %7.2f\n\r", sensorValue.un.magneticField.x);
        tftptr->printf("    y:   %7.2f\n\r", sensorValue.un.magneticField.y);
        tftptr->printf("    z:   %7.2f\n\r", sensorValue.un.magneticField.z);
        break;
    case SH2_LINEAR_ACCELERATION:
        Serial.print("Linear Acceration - x: ");
        Serial.print(sensorValue.un.linearAcceleration.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.linearAcceleration.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.linearAcceleration.z);
        break;
    case SH2_GRAVITY:
        Serial.print("Gravity - x: ");
        Serial.print(sensorValue.un.gravity.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.gravity.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.gravity.z);
        break;
    case SH2_ROTATION_VECTOR:
        Serial.print("Rotation Vector - r: ");
        Serial.print(sensorValue.un.rotationVector.real);
        Serial.print(" i: ");
        Serial.print(sensorValue.un.rotationVector.i);
        Serial.print(" j: ");
        Serial.print(sensorValue.un.rotationVector.j);
        Serial.print(" k: ");
        Serial.println(sensorValue.un.rotationVector.k);

        qw = sensorValue.un.rotationVector.real;
        qx = sensorValue.un.rotationVector.i;
        qy = sensorValue.un.rotationVector.j;
        qz = sensorValue.un.rotationVector.k;

        // Conversion quaternion → yaw/pitch/roll
        yaw = atan2f(2.0f * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz) * 57.2958f;
        pitch = asinf(2.0f * (qw * qy - qx * qz)) * 57.2958f;
        roll = atan2f(2.0f * (qw * qx + qy * qz), qw * qw - qx * qx - qy * qy + qz * qz) * 57.2958f;

        display_angles(yaw, pitch, roll, tftptr);
        // On extrait le statut (les 2 bits de poids faible)
        // 0 = Unreliable (Non fiable)
        // 1 = Accuracy Low (Faible)
        // 2 = Accuracy Medium (Moyenne)
        // 3 = Accuracy High (Maximale)
        calibrationStatus = sensorValue.status & 0x03;
        bno08x_dessinerNiveauVif(pitch, roll, tftptr);
        // bno08x_dessinerBoussole(yaw, tftptr);
        bno08x_dessinerBoussole(yaw, tftptr, true); // Magnétique
        // // Affichage sur l'écran GMT020-02
        // tftptr->setCursor(200, 50);
        // if (calibrationStatus == 3)
        // {
        //     tftptr->setTextColor(ST77XX_GREEN, ST77XX_BLACK);
        //     sh2_saveConfig(); // Sauvegarde dans la mémoire non volatile du BNO08x
        //     tftptr->print("CAL: OK ");
        // }
        // else
        // {
        //     tftptr->setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
        //     tftptr->printf("CAL: %d/3", calibrationStatus);
        // }
        break;
    case SH2_GEOMAGNETIC_ROTATION_VECTOR:
        Serial.print("Geo-Magnetic Rotation Vector - r: ");
        Serial.print(sensorValue.un.geoMagRotationVector.real);
        Serial.print(" i: ");
        Serial.print(sensorValue.un.geoMagRotationVector.i);
        Serial.print(" j: ");
        Serial.print(sensorValue.un.geoMagRotationVector.j);
        Serial.print(" k: ");
        Serial.println(sensorValue.un.geoMagRotationVector.k);

        tftptr->setCursor(0, 160);
        tftptr->setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        tftptr->printf("GRMV r:   %7.2f\n\r", sensorValue.un.geoMagRotationVector.real);
        tftptr->printf("     i:   %7.2f\n\r", sensorValue.un.geoMagRotationVector.i);
        tftptr->printf("     j:   %7.2f\n\r", sensorValue.un.geoMagRotationVector.j);
        tftptr->printf("     k:   %7.2f\n\r", sensorValue.un.geoMagRotationVector.k);
        // You can compute the heading (azimuth) angle from the quaternion as follows:
        //      $$Cap = \operatorname{atan2}(2(qr \cdot qk + qi \cdot qj), 1 - 2(qj^2 + qk^2)) \times \frac{180}{\pi}$$
        cap = atan2(2.0 * (qw * qz + qx * qy), 1.0 - 2.0 * (qy * qy + qz * qz)) * 57.2958;
        tftptr->printf("Heading: %7.2f\n\r", cap);
        break;

    case SH2_GAME_ROTATION_VECTOR:
        Serial.print("Game Rotation Vector - r: ");
        Serial.print(sensorValue.un.gameRotationVector.real);
        Serial.print(" i: ");
        Serial.print(sensorValue.un.gameRotationVector.i);
        Serial.print(" j: ");
        Serial.print(sensorValue.un.gameRotationVector.j);
        Serial.print(" k: ");
        Serial.println(sensorValue.un.gameRotationVector.k);
        break;

    case SH2_STEP_COUNTER:
        Serial.print("Step Counter - steps: ");
        Serial.print(sensorValue.un.stepCounter.steps);
        Serial.print(" latency: ");
        Serial.println(sensorValue.un.stepCounter.latency);
        break;

    case SH2_STABILITY_CLASSIFIER:
        Serial.print("Stability Classification: ");
        stability = sensorValue.un.stabilityClassifier;
        switch (stability.classification)
        {
        case STABILITY_CLASSIFIER_UNKNOWN:
            Serial.println("Unknown");
            break;
        case STABILITY_CLASSIFIER_ON_TABLE:
            Serial.println("On Table");
            break;
        case STABILITY_CLASSIFIER_STATIONARY:
            Serial.println("Stationary");
            break;
        case STABILITY_CLASSIFIER_STABLE:
            Serial.println("Stable");
            break;
        case STABILITY_CLASSIFIER_MOTION:
            Serial.println("In Motion");
            break;
        }
        break;
    case SH2_RAW_ACCELEROMETER:
        Serial.print("Raw Accelerometer - x: ");
        Serial.print(sensorValue.un.rawAccelerometer.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.rawAccelerometer.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.rawAccelerometer.z);
        break;
    case SH2_RAW_GYROSCOPE:
        Serial.print("Raw Gyro - x: ");
        Serial.print(sensorValue.un.rawGyroscope.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.rawGyroscope.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.rawGyroscope.z);
        break;
    case SH2_RAW_MAGNETOMETER:
        Serial.print("Raw Magnetic Field - x: ");
        Serial.print(sensorValue.un.rawMagnetometer.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.rawMagnetometer.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.rawMagnetometer.z);
        break;

    case SH2_SHAKE_DETECTOR:
        Serial.print("Shake Detector - shake detected on axis: ");
        detection = sensorValue.un.shakeDetector;
        switch (detection.shake)
        {
        case SHAKE_X:
            Serial.println("X");
            break;
        case SHAKE_Y:
            Serial.println("Y");
            break;
        case SHAKE_Z:
            Serial.println("Z");
            break;
        default:
            Serial.println("None");
            break;
        }
        break;

    case SH2_PERSONAL_ACTIVITY_CLASSIFIER:

        activity = sensorValue.un.personalActivityClassifier;
        Serial.print("Activity classification - Most likely: ");
        printActivity(activity.mostLikelyState);
        Serial.println("");

        Serial.println("Confidences:");
        // if PAC_OPTION_COUNT is ever > 10, we'll need to
        // care about page
        for (uint8_t i = 0; i < PAC_OPTION_COUNT; i++)
        {
            Serial.print("\t");
            printActivity(i);
            Serial.print(": ");
            Serial.println(activity.confidence[i]);
        }
        break;
    }
    Serial.println("Loop BNO08x end  :" + String(millis(), DEC));
}
