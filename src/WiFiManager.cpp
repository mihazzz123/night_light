#include "WiFiManager.h"

#include "Config.h"
#include "EEPROMHandler.h"

#define DNS_PORT 53

bool WiFiManager::connectToWiFi() {
  if (!config.connected) {
    Serial.println("⚠️ Нет сохраненных настроек Wi-Fi");
    return false;
  }

  Serial.println("💾 Используем сохраненные настройки Wi-Fi");
  Serial.print("🔗 Подключаемся к Wi-Fi: ");
  Serial.println(config.ssid);

  currentState = WIFI_CONNECTING;

  WiFi.begin(config.ssid, config.password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 7) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    currentState = WIFI_CONNECTED;
    // ✅ Отключаем точку доступа
    WiFi.softAPdisconnect(true);
    WiFi.scanDelete();  // освобождает память

    Serial.println("✅ Подключено к Wi-Fi, AP отключен");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.printf("📡 IP адрес: %s\n", WiFi.localIP().toString().c_str());
    return true;
  } else {
    currentState = WIFI_ERROR;
    Serial.println("\n❌ Ошибка подключения к Wi-Fi");
    return false;
  }
}

void WiFiManager::startConfigAP(DNSServer* dns) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.scanNetworks(true);  // запускаем сканирование
  int attempts = 0;
  while (WiFi.scanComplete() == -1 && attempts < 20) {
    delay(100);
    attempts++;
  }

  Serial.println("🔄 Запуск точки доступа для настройки...");
  int foundNetworks = WiFi.scanComplete();
  Serial.printf("🔍 Найдено сетей: %d\n", foundNetworks);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(config.ssidAP, config.passwordAP);

  dns->start(DNS_PORT, "*", WiFi.softAPIP());

  Serial.println("📡 Точка доступа создана: ");
  Serial.print("  📶 SSID: ");
  Serial.println(config.ssidAP);
  Serial.print("  🔑 Пароль для отладки: ");
  Serial.println(config.passwordAP);
  Serial.printf("📡 IP адрес: ");
  Serial.println(WiFi.softAPIP());

  currentState = WIFI_AP_MODE;
}

void WiFiManager::initWiFi(DNSServer* dns, ESP8266WebServer* server) {
  if (!connectToWiFi()) {
    startConfigAP(dns);
  }
}

void WiFiManager::checkWiFiStatus(unsigned long now) {
  static unsigned long lastWifiCheck = 0;

  if (now - lastWifiCheck > 10000) {
    if (WiFi.status() != WL_CONNECTED && currentState == WIFI_CONNECTED &&
        WiFi.getMode() != WIFI_AP) {
      Serial.println("❌ Потеряно соединение с Wi-Fi");
      currentState = WIFI_DISCONNECTED;
    }
    lastWifiCheck = now;
  }
}

bool WiFiManager::handleConnectWiFi(ESP8266WebServer* server) {
  if (!server->hasArg("ssid") || !server->hasArg("password")) {
    Serial.println("❌ Не переданы параметры подключения");
    return false;
  }
  String newSSID = server->arg("ssid");
  String newPassword = server->arg("password");

  Serial.println("📶 Выбрана сеть: " + newSSID);
  WiFi.begin(newSSID.c_str(), newPassword.c_str());

  currentState = WIFI_CONNECTING;
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 7) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    currentState = WIFI_CONNECTED;
    config.connected = true;
    strncpy(config.ssid, newSSID.c_str(), sizeof(config.ssid) - 1);
    config.ssid[sizeof(config.ssid) - 1] = '\0';
    strncpy(config.password, newPassword.c_str(), sizeof(config.password) - 1);
    config.password[sizeof(config.password) - 1] = '\0';
    saveAppConfig();
    Serial.println("\n✅ Подключено!");
    Serial.print("SSID: ");
    Serial.printf("📡 IP адрес: %s\n", WiFi.localIP().toString().c_str());
    Serial.println(WiFi.localIP());
    Serial.println("Restarting...");
    ESP.restart();
    return true;
  } else {
    currentState = WIFI_ERROR;
    Serial.println("\n❌ Ошибка подключения");
    return false;
  }
}