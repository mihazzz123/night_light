#ifndef WEBROUTES_H
#define WEBROUTES_H

#include <Adafruit_NeoPixel.h>
#include <ESP8266WebServer.h>

#include "Config.h"

class WebRoutes {
 public:
  static void registerRoutes(ESP8266WebServer* server,
                             Adafruit_NeoPixel* strip);

 private:
  static void handleRoot();
  static void handleSetColor();
  static void handleUpdateWiFi();
  static void handleRestart();
  static void handleConnectWiFi();
  static void handleScanNetworks();

  static ESP8266WebServer* _server;
  static Adafruit_NeoPixel* _strip;
};

#endif  // WEBROUTES_H