#ifndef LEDHANDLER_H
#define LEDHANDLER_H

#include <Adafruit_NeoPixel.h>

#include "Config.h"

class LEDHandler {
 public:
  static void initStrip(Adafruit_NeoPixel* strip);
  static void updateStatusLED(Adafruit_NeoPixel* strip, unsigned long* timer1);
  static void setColor(Adafruit_NeoPixel* strip, const String& color);

 private:
  static void handleWiFiDisconnected(Adafruit_NeoPixel* strip);
  static void handleWiFiConnecting(Adafruit_NeoPixel* strip);
  static void handleWiFiConnected(Adafruit_NeoPixel* strip);
  static void handleWiFiError(Adafruit_NeoPixel* strip);
};

#endif  // LEDHANDLER_H