#include "Communication.hpp"
#include "Log.hpp"

// Variables dynamiques qui changeront selon le lieu
#ifdef MODE_TCP
// IPAddress targetIP;
String targetIP = "";
WiFiClient Communication::clientCapteur;
#endif

#ifdef MODE_MAC
uint8_t Communication::receiver[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0x12};

static int startSend;
static int endSend;

static void dataReceived(uint8_t *address, uint8_t *data, uint8_t len, signed int rssi, bool broadcast)
{
    // Serial.print ("Received: ");
    // //Serial.printf ("%.*s\n", len, data);
    // Serial.printf ("%d bytes\n", len);
    // Serial.printf ("RSSI: %d dBm\n", rssi);
    // Serial.printf ("From: " MACSTR "\n", MAC2STR (address));
    // Serial.printf ("%s\n", broadcast ? "Broadcast" : "Unicast");
}

static void dataSent(uint8_t *address, uint8_t status)
{
    endSend = micros();
    static unsigned int counter = 0;
    counter++;
    static int sum = 0;
    sum += (endSend - startSend);
    int avg = sum / counter;

    if (counter % 20 == 0)
    {
        Serial.printf("Message sent in %d us\n", endSend - startSend);
        Serial.printf("Average time: %d us\n", avg);
        if (status == COMMS_SEND_OK)
        {
            Serial.println(">>>>>>>>>> Message sent");
        }
        else
        {
            Serial.println(">>>>>>>>>> Message not sent");
        }
    }
}
#endif

void Communication::setup(bool networkHP = true)
{
    if (networkHP)
    {
        // 🏠 À LA MAISON
        Serial.println("\n🏠 Connecté au Wi-Fi maison !");
        Log::addLog("🏠 Connecté au Wi-Fi maison ! IP du S3 : " + WiFi.localIP().toString());
#ifdef MODE_TCP
        // ⚠️ Remplplace ici par l'IP que ton S3 affiche quand il est connecté à ta box !
        targetIP = "192.168.0.228";
#endif
    }
    else
    {
        // 🌌 À L'EXTÉRIEUR
        Serial.println("\n❌ Wi-Fi maison introuvable. Connexion au S3 Nomade...");
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        WiFi.begin(ap_ssid, ap_password);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\n✅ Connecté directement à l'ESP32-S3 !");

#ifdef MODE_TCP
        // En mode AP, l'IP du serveur (le S3) est TOUJOURS 192.168.4.1
        targetIP = "192.168.4.1";
#endif
    }
#ifdef MODE_MAC
    quickEspNow.onDataRcvd(dataReceived);
    quickEspNow.onDataSent(dataSent);
#ifdef ESP32
    quickEspNow.setWiFiBandwidth(WIFI_IF_STA, WIFI_BW_HT20); // Only needed for ESP32 in case you need coexistence with ESP8266 in the same network
#endif                                                       // ESP32
    quickEspNow.begin(1, 0, false);                          // If you use no connected WiFi channel should be specified
#endif
}
bool Communication::openConnection()
{
#ifdef MODE_TCP
    if (!clientCapteur.connect(targetIP.c_str(), portTCP))
    {
        Serial.println("❌ NON Connecté au S3 !");
        Log::addLog("❌ NON Connecté au S3 !");
        return false;
    }
#endif
    Serial.println("🤝 Connecté au S3 !");
    Log::addLog("🤝 Connecté au S3 !", true);
    return true;
}

void Communication::closeConnection()
{
#ifdef MODE_TCP
    clientCapteur.flush();
    clientCapteur.stop();
#endif
    Serial.println("🔌 Déconnecté du S3 !");
    Log::addLog("🔌 Déconnecté du S3 !", true);
}

bool Communication::send(CapteursMessage *incomingData)
{
    Log::addLog("communication::send()", true);
#ifdef MODE_TCP
    clientCapteur.write((uint8_t *)incomingData, sizeof(CapteursMessage));
#endif
#ifdef MODE_MAC
    startSend = micros();
    if (quickEspNow.readyToSendData())
        if (!quickEspNow.send(receiver, (uint8_t *)incomingData, sizeof(CapteursMessage)))
        {
            endSend = micros();
            Serial.println("❌ NON envoyé au S3 !");
            Log::addLog("❌ NON envoyé au S3 !");
            return false;
        }
        else
        {
            endSend = micros();
            Serial.println("✅ Envoyé au S3 !");
            Log::addLog("✅ Envoyé au S3 !");
            return true;
        }
    else
    {
        Serial.println("❌ NON prêt à envoyer au S3 !");
        Log::addLog("❌ NON prêt à envoyer au S3 !");
        return false;
    }
#endif
    return true;
}