#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#include "EEPROMHandler.h"

// 🏷️ Перечисление состояний Wi-Fi подключения
enum WState {
  WIFI_DISCONNECTED,
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_AP_MODE,
  WIFI_ERROR
};

// 📡 Конфигурация приложения (теперь использует EEPROM)
extern EEPROMConfig config;
extern WState currentState;
extern bool ready;

// 🔧 Функции для работы с конфигурацией
void loadAppConfig();
void saveAppConfig();
void updateWiFiConfig(const String& ssid, const String& password);
void updateColorConfig(const String& color);

#endif  // CONFIG_H