// server - récepteur

#include <WiFi.h>

// Configuration du Wi-Fi existant (Maison / Atelier)
const char *home_ssid = "WIFI_MAISON";
const char *home_pass = "MOT_DE_PASSE_MAISON";

// Configuration du Point d'Accès autonome (SoftAP)
const char *ap_ssid = "ESP32-Server-Net";
const char *ap_pass = "123456789"; // Minimum 8 caractères

WiFiServer server(8080);

void setup()
{
    Serial.begin(115200);

    // 1. Tenter la connexion au Wi-Fi existant
    WiFi.mode(WIFI_STA);
    WiFi.begin(home_ssid, home_pass);
    Serial.print("Connexion au Wi-Fi existant...");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    { // Attente de ~10 sec
        delay(500);
        Serial.print(".");
        attempts++;
    }

    // 2. Si le Wi-Fi existant n'est pas trouvé, passer en mode Point d'Accès (SoftAP)
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nConnecté au réseau Wi-Fi existant !");
        Serial.print("IP Serveur : ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\nWi-Fi non trouvé. Bascule en mode Point d'Accès (SoftAP)...");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid, ap_pass);
        Serial.print("Réseau AP créé : ");
        Serial.println(ap_ssid);
        Serial.print("IP Serveur (SoftAP) : ");
        Serial.println(WiFi.softAPIP()); // Généralement 192.168.4.1
    }

    server.begin();
}

void loop()
{
    WiFiClient client = server.available();

    if (client)
    {
        Serial.println("Client TCP connecté !");
        while (client.connected())
        {
            if (client.available())
            {
                String msg = client.readStringUntil('\n');
                Serial.print("Message reçu : ");
                Serial.println(msg);

                client.println("ACK: OK"); // Confirmation de réception
            }
        }
        client.stop();
    }
}