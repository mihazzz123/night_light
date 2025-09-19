#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
#include <Adafruit_NeoPixel.h>
#include "led_control.h"

#define LED_PIN D2
#define LED_COUNT 8

ESP8266WebServer server;
AutoConnect portal(server);
AutoConnectConfig config;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum WState
{
  WIFI_DISCONNECTED,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_ERROR
};

WState currentState = WIFI_DISCONNECTED;
unsigned long lastBlink = 0;
bool ledOn = false;

// 📡 Страница состояния сети
void handleStatusPage()
{
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Состояние сети</title></head><body>";
  html += "<h2>Состояние подключения</h2>";

  if (WiFi.status() == WL_CONNECTED)
  {
    html += "<p><strong>SSID:</strong> " + WiFi.SSID() + "</p>";
    html += "<p><strong>IP:</strong> " + WiFi.localIP().toString() + "</p>";
    html += "<p><strong>RSSI:</strong> " + String(WiFi.RSSI()) + " dBm</p>";
    html += "<p><a href='/led'>Перейти к управлению</a></p>";
  }
  else
  {
    html += "<p>❌ Не подключено к сети Wi-Fi</p>";
    html += "<p><a href='/_ac'>Настроить подключение</a></p>";
  }

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(115200);
  strip.begin();
  strip.show(); // очистка

  // ⚙️ Настройки AutoConnect
  config.apid = "ESP_Config";
  config.psk = "11111111";
  config.autoRise = true;
  config.immediateStart = true;
  config.retainPortal = true;
  config.homeUri = "/led"; // ✅ автоматический переход на /led после подключения

  portal.config(config);

  // 🌐 Роуты
  server.on("/led", handleLedPage);
  server.on("/setColor", handleSetColor);
  server.on("/status", handleStatusPage);

  // 🚀 Запуск портала
  currentState = WIFI_CONNECTING;
  if (portal.begin())
  {
    currentState = WIFI_CONNECTED;
    Serial.println("✅ Подключено! IP: " + WiFi.localIP().toString());
  }
  else
  {
    currentState = WIFI_DISCONNECTED;
    Serial.println("🕸 Captive Portal активен. Ожидание подключения...");
  }
}

void updateStatusLED()
{
  unsigned long now = millis();

  switch (currentState)
  {
  case WIFI_DISCONNECTED:
    if (now - lastBlink > 500)
    {
      ledOn = !ledOn;
      uint32_t color = ledOn ? strip.Color(255, 0, 0) : strip.Color(0, 0, 0);
      for (int i = 0; i < strip.numPixels(); i++)
        strip.setPixelColor(i, color);
      strip.show();
      lastBlink = now;
    }
    break;

  case WIFI_CONNECTING:
    static uint8_t brightness = 0;
    static int8_t direction = 5;
    brightness += direction;
    if (brightness >= 255 || brightness <= 0)
      direction = -direction;
    for (int i = 0; i < strip.numPixels(); i++)
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    strip.show();
    delay(20);
    break;

  case WIFI_ERROR:
    if (now - lastBlink > 200)
    {
      ledOn = !ledOn;
      uint32_t color = ledOn ? strip.Color(255, 0, 0) : strip.Color(0, 0, 0);
      for (int i = 0; i < strip.numPixels(); i++)
        strip.setPixelColor(i, color);
      strip.show();
      lastBlink = now;
    }
    break;
  }
}

void loop()
{
  portal.handleClient();
  if (currentState != WIFI_CONNECTED)
  {
    updateStatusLED(); // 🔄 обновляем индикацию
  }
}
