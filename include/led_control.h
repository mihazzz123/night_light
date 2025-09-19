#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

extern ESP8266WebServer server;
extern Adafruit_NeoPixel strip;

void handleLedPage();
void handleSetColor();

#endif
