// client - émetteur

#include <WiFi.h>

// Identifiants Wi-Fi Maison
const char *home_ssid = "WIFI_MAISON";
const char *home_pass = "MOT_DE_PASSE_MAISON";

// Identifiants Point d'Accès de l'ESP32 Serveur
const char *ap_ssid = "ESP32-Server-Net";
const char *ap_pass = "123456789";

// Port TCP
const uint16_t serverPort = 8080;

// Variables dynamiques d'IP
IPAddress targetIP;

void setup()
{
    Serial.begin(115200);

    // 1. Tenter le Wi-Fi existant
    WiFi.mode(WIFI_STA);
    WiFi.begin(home_ssid, home_pass);
    Serial.print("Connexion au Wi-Fi existant...");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    // 2. Si échec, se connecter au Point d'Accès du Serveur
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConnecté au Wi-Fi existant.");
        // Remplacez cette IP par l'IP attribuée au Serveur sur votre box
        targetIP.fromString("192.168.1.50");
    }
    else
    {
        Serial.println("\nWi-Fi principal indisponible. Connexion à l'ESP32 Serveur...");
        WiFi.begin(ap_ssid, ap_pass);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nConnecté au Point d'Accès du Serveur !");
        targetIP = WiFi.gatewayIP(); // L'IP du serveur AP est toujours la passerelle (ex: 192.168.4.1)
    }
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;

        if (client.connect(targetIP, serverPort))
        {
            Serial.print("Envoi des données à ");
            Serial.println(targetIP);

            client.println("Données de capteur ESP32 Client");

            // Attente réponse
            unsigned long timeout = millis();
            while (client.available() == 0 && millis() - timeout < 2000)
                ;

            if (client.available())
            {
                String rep = client.readStringUntil('\n');
                Serial.print("Réponse Serveur : ");
                Serial.println(rep);
            }
            client.stop();
        }
        else
        {
            Serial.println("Échec de la connexion TCP au serveur.");
        }
    }

    delay(3000);
}