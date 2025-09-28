#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "Config.h"
#include "EEPROMHandler.h"
#include "LEDHandler.h"
#include "WebRoutes.h"
#include "WiFiManager.h"

// 🔧 Определение констант
#define LED_PIN D2
#define LED_COUNT 8

// 🌐 Глобальные объекты
DNSServer dns;
ESP8266WebServer server(80);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// 🔄 Переменные состояния
unsigned long timer1 = 0;

bool ready = false;

void setup() {
  Serial.begin(9600);
  delay(1000);  // Даем время Serial для инициализации
  Serial.println("\n🚀 Запуск системы...");

  // 💾 Инициализация EEPROM
  EEPROMHandler::begin();

  // 💾 Загрузка конфигурации из EEPROM
  loadAppConfig();

  // Увеличиваем счетчик перезагрузок и сохраняем
  config.bootCount++;
  saveAppConfig();

  // 💡 Инициализация светодиодной ленты
  LEDHandler::initStrip(&strip);

  // 🎨 Восстановление последнего цвета
  LEDHandler::setColor(&strip, String(config.lastColor));

  // 🌐 Инициализация Wi-Fi с настройками из EEPROM
  WiFiManager::initWiFi(&dns, &server);

  // 🌐 Регистрация обработчиков веб-сервера
  WebRoutes::registerRoutes(&server, &strip);

  // 🚀 Запуск веб-сервера
  server.begin();
  Serial.println("🌐 Веб-сервер запущен!");
}

void loop() {
  unsigned long now = millis();

  dns.processNextRequest();
  server.handleClient();

  // 🔄 Обновление индикации каждые 500 мс
  if (now - timer1 > 500 && !ready) {
    LEDHandler::updateStatusLED(&strip, &timer1);
  }

  // 🔍 Периодическая проверка состояния Wi-Fi
  WiFiManager::checkWiFiStatus(now);
}