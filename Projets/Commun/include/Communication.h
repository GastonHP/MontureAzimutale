#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_BNO08x.h>

// Configuration pour le mode extérieur (Point d'Accès)
#define ap_ssid "Commande_net"
#define ap_password "12345678" // 8 caractères min.

#define protocolVersion 4
#define udpLocalPort 4210 // Port d'écoute local

