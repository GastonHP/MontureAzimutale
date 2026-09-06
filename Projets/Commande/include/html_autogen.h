// ⚠️ FICHIER GÉNÉRÉ AUTOMATIQUEMENT - NE PAS MODIFIER DIRECTEMENT
// Modifie plutôt le fichier /html/index.html

#ifndef HTML_AUTOGEN_H
#define HTML_AUTOGEN_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
<!-- test v -->
<head>
    <title>Telescope Control Center</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: sans-serif;
            text-align: center;
            background: #1a1a1a;
            color: #eee;
        }

        .card {
            background: #333;
            padding: 15px;
            margin: 10px;
            border-radius: 10px;
            display: inline-block;
            min-width: 150px;
        }

        .value {
            font-size: 1.2em;
            color: #00ffcc;
            font-weight: bold;
        }

        .delta {
            font-size: 1.2em;
            color: #9dff00;
            font-weight: bold;
        }

        .btn {
            padding: 20px;
            font-size: 20px;
            width: 80px;
            margin: 5px;
            cursor: pointer;
            border-radius: 10px;
            border: none;
            background: #444;
            color: white;
        }

        .btn:active {
            background: #666;
        }

        .log {
            background: #222;
            padding: 10px;
            margin: 5px;
            border-radius: 5px;
            text-align: left;
            font-family: monospace;
            color: #0f0;
        }
    </style>
</head>

<body>
    <table>
        <tr>
            <td>
                <table style="margin: 0 auto; border-width: 5px; border-color: #555; border-style: solid;">
                    <tr style="background: #555;">
                        <td></td>
                        <td>Yaw</td>
                        <td>Delta AZ</td>
                        <td>Pitch</td>
                        <td>Delta ALT</td>
                        <td>Roll</td>
                        <td>Accuracy</td>
                        <td>Precision</td>
                        <td>Timestamp</td>
                    </tr>
                    <tr style="background: #333;">
                        <td>ARVR</td>
                        <td id="arvr_yaw" class="value">0.0°</td>
                        <td id="arvr_deltaAZ" class="delta">0.0°</td>
                        <td id="arvr_pitch" class="value">0.0°</td>
                        <td id="arvr_deltaALT" class="delta">0.0°</td>
                        <td id="arvr_roll" class="value">0.0°</td>
                        <td id="arvr_accuracy" class="value">0.0</td>
                        <td id="arvr_precision" class="value">0</td>
                        <td id="arvr_timestamp" class="value">TS: --</td>
                    </tr>
                    <tr style="background: #333;">
                        <td>RV</td>
                        <td id="rotation_vector_yaw" class="value">0.0°</td>
                        <td id="rotation_vector_deltaAZ" class="delta">0.0°</td>
                        <td id="rotation_vector_pitch" class="value">0.0°</td>
                        <td id="rotation_vector_deltaALT" class="delta">0.0°</td>
                        <td id="rotation_vector_roll" class="value">0.0°</td>
                        <td id="rotation_vector_accuracy" class="value">0.0</td>
                        <td id="rotation_vector_precision" class="value">0</td>
                        <td id="rotation_vector_timestamp" class="value">TS: --</td>
                    </tr>
                    <tr style="background: #333;">
                        <td>GRV</td>
                        <td id="game_rotation_vector_yaw" class="value">0.0°</td>
                        <td id="game_rotation_vector_deltaAZ" class="delta">0.0°</td>
                        <td id="game_rotation_vector_pitch" class="value">0.0°</td>
                        <td id="game_rotation_vector_deltaALT" class="delta">0.0°</td>
                        <td id="game_rotation_vector_roll" class="value">0.0°</td>
                        <td id="game_rotation_vector_accuracy" class="value">0.0</td>
                        <td id="game_rotation_vector_precision" class="value">0</td>
                        <td id="game_rotation_vector_timestamp" class="value">TS: --</td>
                    </tr>
                </table>

                <table style="margin: 0 auto;">
                    <tr>
                        <td>
                            <div class="card">
                                <div>Batterie</div>
                                <div id="batt" class="value">-- V</div>
                            </div>
                            <div class="card">
                                <div>Sats</div>
                                <div id="sats" class="value">0</div>
                            </div>
                            <button class="btn" style="background:#37ad51; width: 130px;"
                                onclick="move('origine')">origine</button>
                            <br>
                            <div style="margin-top:20px;">
                                <button class="btn" onclick="move('up')">up</button><br>
                                <button class="btn" onclick="move('left')">left</button>
                                <button class="btn" style="background:#d32f2f" onclick="move('stop')">stop</button>
                                <button class="btn" onclick="move('right')">right</button><br>
                                <button class="btn" onclick="move('down')">down</button><br>
                            </div>
                            <div style="margin-top:20px;">

                                <button class="btn" style="background:#4f3ba3;width: 130px;"
                                    onclick="move('calibrateAZ')">calibration AZ</button>
                                <button class="btn" style="background:#4f3ba3;width: 130px;"
                                    onclick="move('calibrateALT')">calibration ALT</button>
                                <button class="btn" style="background:#4f3ba3;width: 130px;"
                                    onclick="move('calibrate')">calibration</button>
                                <button class="btn" style="background:#4f3ba3;width: 130px;"
                                    onclick="move('calibrateMVT')">calibration<br>mouvement</button><br>
                                <button class="btn" style="background:#4f3ba3;width: 130px;"
                                    onclick="reboot()">REBOOT</button>
                            </div>
                        </td>
                    </tr>
                    <tr>
                        <td>Channel : </td>
                        <td id="channel">cccccccccc</td>
                    </tr>
                    <tr>
                        <td>SSID : </td>
                        <td id="ssid">cccccccccc</td>
                    </tr>
                </table>
            </td>
            <td style="vertical-align: top; padding-left: 20px;">
                <div id="log">lignes log</div>
            </td>
        </tr>
    </table>

    <script>
        function reboot() { fetch('/reboot'); }
        function move(dir) { fetch('/control?dir=' + dir); }
        // Écoute les mises à jour automatiques du serveur
        if (!!window.EventSource) {
            var source = new EventSource('/events');
            source.addEventListener('data_update', function (e) {
                var obj = JSON.parse(e.data);
                document.getElementById('channel').innerHTML = obj.channel;
                document.getElementById('ssid').innerHTML = obj.ssid;
                document.getElementById('arvr_yaw').innerHTML = obj.arvr_yaw + "&deg;"; // ° est un caractère spécial, on peut aussi utiliser "°"
                document.getElementById('arvr_pitch').innerHTML = obj.arvr_pitch + "&deg;";
                document.getElementById('arvr_roll').innerHTML = obj.arvr_roll + "&deg;";
                document.getElementById('arvr_accuracy').innerHTML = obj.arvr_accuracy;
                document.getElementById('arvr_precision').innerHTML = obj.arvr_precision;
                document.getElementById('arvr_deltaALT').innerHTML = obj.arvr_deltaALT;
                document.getElementById('arvr_deltaAZ').innerHTML = obj.arvr_deltaAZ;
                document.getElementById('arvr_timestamp').innerHTML = obj.arvr_timestamp;

                document.getElementById('rotation_vector_yaw').innerHTML = obj.rotation_vector_yaw + "&deg;";
                document.getElementById('rotation_vector_pitch').innerHTML = obj.rotation_vector_pitch + "&deg;";
                document.getElementById('rotation_vector_roll').innerHTML = obj.rotation_vector_roll + "&deg;";
                document.getElementById('rotation_vector_accuracy').innerHTML = obj.rotation_vector_accuracy;
                document.getElementById('rotation_vector_precision').innerHTML = obj.rotation_vector_precision;
                document.getElementById('rotation_vector_deltaAZ').innerHTML = obj.rotation_vector_deltaAZ;
                document.getElementById('rotation_vector_deltaALT').innerHTML = obj.rotation_vector_deltaALT;
                document.getElementById('rotation_vector_timestamp').innerHTML = obj.rotation_vector_timestamp;

                document.getElementById('game_rotation_vector_yaw').innerHTML = obj.game_rotation_vector_yaw + "&deg;";
                document.getElementById('game_rotation_vector_pitch').innerHTML = obj.game_rotation_vector_pitch + "&deg;";
                document.getElementById('game_rotation_vector_roll').innerHTML = obj.game_rotation_vector_roll + "&deg;";
                document.getElementById('game_rotation_vector_accuracy').innerHTML = obj.game_rotation_vector_accuracy;
                document.getElementById('game_rotation_vector_precision').innerHTML = obj.game_rotation_vector_precision;
                document.getElementById('game_rotation_vector_deltaAZ').innerHTML = obj.game_rotation_vector_deltaAZ;
                document.getElementById('game_rotation_vector_deltaALT').innerHTML = obj.game_rotation_vector_deltaALT;
                document.getElementById('game_rotation_vector_timestamp').innerHTML = obj.game_rotation_vector_timestamp;

                document.getElementById('batt').innerHTML = obj.batt + " V";
                document.getElementById('sats').innerHTML = obj.sats;
                document.getElementById('log').innerHTML = obj.log; // Affiche la ligne de log la plus récente
            }, false);
        }
    </script>
</body>

</html>)rawliteral";

#endif
