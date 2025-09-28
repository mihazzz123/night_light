#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "Config.h"

// Структура для хранения данных wifi в EEPROM
struct WiFiCredentials {
  char ssid;
  char password;
};

class WiFiManager {
 public:
  static void initWiFi(DNSServer* dns, ESP8266WebServer* server);
  static void checkWiFiStatus(unsigned long now);
  static bool handleConnectWiFi(ESP8266WebServer* server);

 private:
  static bool connectToWiFi();
  static void startConfigAP(DNSServer* dns);
};

#endif  // WIFIMANAGER_H