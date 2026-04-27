#include "WebServer.hpp"
#include "Telescope.hpp"
#include "Batterie.hpp"

AsyncWebServer WebServer::server(80);
GPSManager *WebServer::gpsManager = nullptr;

// Page Web stockée en Flash
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Telescope Control Center</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; text-align: center; background: #1a1a1a; color: #eee; }
        .card { background: #333; padding: 15px; margin: 10px; border-radius: 10px; display: inline-block; min-width: 150px; }
        .value { font-size: 1.2em; color: #00ffcc; font-weight: bold; }
        .btn { padding: 20px; font-size: 20px; width: 80px; margin: 5px; cursor: pointer; border-radius: 10px; border: none; background: #444; color: white; }
        .btn:active { background: #666; }
    </style>
</head>
<body>
    <h1>Telescope S3</h1>
    
    <div class="card"><div>Azimut/yaw<br>Direction</div><div id="azi" class="value">0.0°</div></div>
    <div class="card"><div>Inclinaison/roll<br>Altitude</div><div id="alt" class="value">0.0°</div></div>
    <div class="card"><div>Roulis/pitch<br>Angle</div><div id="pitch" class="value">0.0°</div></div>
    <br>
    <div class="card"><div>Accuracy</div><div id="accuracy" class="value">0.0</div></div>
    <div class="card"><div>Pr&eacute;cision</div><div id="precision" class="value">0</div></div>    
    <div class="card"><div>Batterie</div><div id="batt" class="value">-- V</div></div>
    <div class="card"><div>Sats</div><div id="sats" class="value">0</div></div>
    <br>
    <div class="card"><div>deltaAZ</div><div id="deltaAZ" class="value">0</div></div>
    <div class="card"><div>deltaALT</div><div id="deltaALT" class="value">0</div></div>
    <div style="margin-top:20px;">
        <button class="btn" onclick="move('up')">up</button><br>
        <button class="btn" onclick="move('left')">left</button>
        <button class="btn" style="background:#d32f2f" onclick="move('stop')">stop</button>
        <button class="btn" onclick="move('right')">right</button><br>
        <button class="btn" onclick="move('down')">down</button><br>
        <br>
        <button class="btn" onclick="move('calibrate')">calibration</button><br>
    </div>

    <script>
        function move(dir) { fetch('/control?dir=' + dir); }

        // Écoute les mises à jour automatiques du serveur
        if (!!window.EventSource) {
            var source = new EventSource('/events');
            source.addEventListener('data_update', function(e) {
                var obj = JSON.parse(e.data);
                document.getElementById('alt').innerHTML = obj.alt + "&deg;"; // ° est un caractère spécial, on peut aussi utiliser "°"
                document.getElementById('azi').innerHTML = obj.azi + "&deg;";
                document.getElementById('pitch').innerHTML = obj.pitch + "&deg;";
                document.getElementById('accuracy').innerHTML = obj.accuracy;
                document.getElementById('precision').innerHTML = obj.precision;
                document.getElementById('batt').innerHTML = obj.batt + " V";
                document.getElementById('sats').innerHTML = obj.sats;
                document.getElementById('deltaALT').innerHTML = obj.deltaALT;
                document.getElementById('deltaAZ').innerHTML = obj.deltaAZ;
            }, false);
        }
    </script>
</body>
</html>
)rawliteral";

AsyncEventSource events("/events");

void WebServer::setup(GPSManager *manager)
{
    gpsManager = manager;
    // Route pour la page d'accueil
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    // Route pour les commandes de mouvement
    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("dir")) 
        {
            String dir = request->getParam("dir")->value();
            Serial.println("Commande reçue: " + dir);
            // Ici, vous pouvez appeler les fonctions de contrôle du télescope en fonction de 'dir'
            if (dir == "up")    Telescope::steps(0,100);    // 100 pas en Altitude
            if (dir == "down")  Telescope::steps(0,-100);
            if (dir == "left")  Telescope::steps(-100,0);   // 100 pas en Azimut
            if (dir == "right") Telescope::steps(100,0);
            if (dir == "stop")  Telescope::stop();            
            if(dir == "calibrate") Telescope::calibrate();
        }
        request->send(200, "text/plain", "OK"); });

    server.addHandler(&events);
    server.begin();
}

static bool stopped = false;

void WebServer::stop()
{
    if (!stopped)
    {
        Serial.println("Arrêt du serveur Web...");
        stopped = true;
        server.end();
    }
}

void WebServer::loop()
{
    if (stopped)
        return;

    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 500)
    {
        // Calcul batterie (pont 300k/51k)
        float batt = Batterie::lireTension(); // * (300.0 + 51.0) / 51.0;

        // Récupération angles BNO (à adapter selon votre lib)
        EulerAngles angles = Telescope::getCurrentAngles();
        float currentAlt = angles.roll;       // Inclinaison
        float currentAzi = angles.yaw;        // Azimut
        float currentPitch = angles.pitch;    // Non utilisé pour l'instant
        float accuracy = Telescope::accuracy; // Précision du capteur

        // Création du message JSON pour le Web
        String json = "{";
        json += "\"alt\":\"" + String(currentAlt, 1) + "\",";
        json += "\"azi\":\"" + String(currentAzi, 1) + "\",";
        json += "\"pitch\":\"" + String(currentPitch, 1) + "\",";
        json += "\"accuracy\":\"" + String(accuracy, 1) + "\",";
        json += "\"precision\":\"" + String(Telescope::precision) + "\",";
        json += "\"batt\":\"" + String(batt, 1) + "\",";
        json += "\"sats\":\"" + String(gpsManager->satellites()) + "\",";
        json += "\"deltaALT\":\"" + String(Telescope::deltaALT.toString()) + "\",";
        json += "\"deltaAZ\":\"" + String(Telescope::deltaAZ.toString()) + "\"";
        json += "}";

        // Envoi aux clients Web connectés
        events.send(json.c_str(), "data_update", millis());

        lastUpdate = millis();
    }
}
