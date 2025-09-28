#include "LEDHandler.h"

#include "Config.h"

void LEDHandler::initStrip(Adafruit_NeoPixel* strip) {
  strip->begin();
  strip->show();  // Очистка светодиодов
}

void LEDHandler::setColor(Adafruit_NeoPixel* strip, const String& color) {
  uint32_t rgbColor;

  if (color == "red")
    rgbColor = strip->Color(255, 0, 0);
  else if (color == "green")
    rgbColor = strip->Color(0, 255, 0);
  else if (color == "blue")
    rgbColor = strip->Color(0, 0, 255);
  else if (color == "yellow")
    rgbColor = strip->Color(255, 255, 0);
  else if (color == "purple")
    rgbColor = strip->Color(255, 0, 255);
  else if (color == "white")
    rgbColor = strip->Color(255, 255, 255);
  else
    rgbColor = strip->Color(0, 0, 0);  // Выключить

  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, rgbColor);
  }
  strip->show();

  Serial.println("🎨 Установлен цвет: " + color);
}

void LEDHandler::updateStatusLED(Adafruit_NeoPixel* strip,
                                 unsigned long* timer1) {
  *timer1 = millis();

  switch (currentState) {
    case WIFI_DISCONNECTED:
      handleWiFiDisconnected(strip);
      break;
    case WIFI_CONNECTING:
      handleWiFiConnecting(strip);
      break;
    case WIFI_CONNECTED:
      handleWiFiConnected(strip);
      break;
    case WIFI_AP_MODE:
      handleWiFiConnected(strip);
      break;
    case WIFI_ERROR:
      handleWiFiError(strip);
      break;
  }
}

void LEDHandler::handleWiFiDisconnected(Adafruit_NeoPixel* strip) {
  // 🔴 Wi-Fi отключен - постоянный красный
  ready = false;
  uint32_t color = strip->Color(255, 0, 0);
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, color);
  }
  strip->show();
}

void LEDHandler::handleWiFiConnecting(Adafruit_NeoPixel* strip) {
  // 🟢 Подключение в процессе - пульсирующий зеленый
  static uint8_t brightness = 0;
  static int8_t direction = 5;

  brightness += direction;
  if (brightness >= 255 || brightness <= 0) {
    direction = -direction;
  }

  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, strip->Color(0, brightness, 0));
  }
  strip->show();
}

void LEDHandler::handleWiFiConnected(Adafruit_NeoPixel* strip) {
  // 🔵 Подключено - постоянный синий
  uint32_t color = strip->Color(0, 0, 255);
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, color);
  }
  strip->show();
}

void LEDHandler::handleWiFiError(Adafruit_NeoPixel* strip) {
  // ⚠️ Ошибка - мигающий красный
  ready = !ready;
  uint32_t color = ready ? strip->Color(255, 0, 0) : strip->Color(0, 0, 0);
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, color);
  }
  strip->show();
}
