#ifndef EEPROMHANDLER_H
#define EEPROMHANDLER_H

#include <Arduino.h>
#include <EEPROM.h>

// 🔧 Структура для хранения конфигурации в EEPROM
struct EEPROMConfig {
  char ssid[32];        // SSID Wi-Fi сети
  char password[64];    // Пароль Wi-Fi сети
  char ssidAP[32];      // SSID точки доступа
  char passwordAP[64];  // Пароль точки доступа
  char lastColor[16];   // Последний цвет
  bool connected;       // Флаг подключения
  uint16_t bootCount;   // Счётчик перезагрузок
  uint8_t brightness;   // Яркость (0–255)
};

class EEPROMHandler {
 public:
  static void begin();
  static void loadConfig(EEPROMConfig& config);
  static bool saveConfig(const EEPROMConfig& config);
  static void printConfig(const EEPROMConfig& config);

  // Вспомогательные методы
  static EEPROMConfig getDefaultConfig();

 private:
  static const uint32_t EEPROM_SIZE = 512;
  static const uint32_t CONFIG_ADDRESS = 0;
  static const uint8_t CONFIG_VERSION = 0x01;
};

#endif  // EEPROMHANDLER_H