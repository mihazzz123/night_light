#include "EEPROMHandler.h"

void EEPROMHandler::begin() {
  //   EEPROM.begin(sizeof(EEPROMConfig));
  EEPROM.begin(512);
  delay(1000);  // Даем время на инициализацию
  Serial.println("📀 EEPROM инициализирована");
}

EEPROMConfig EEPROMHandler::getDefaultConfig() {
  EEPROMConfig config;

  // Устанавливаем значения по умолчанию
  strncpy(config.ssidAP, "ESP_Config", sizeof(config.ssidAP));
  config.ssidAP[sizeof(config.ssidAP) - 1] = '\0';

  strncpy(config.passwordAP, "11111111", sizeof(config.passwordAP));
  config.passwordAP[sizeof(config.passwordAP) - 1] = '\0';

  strncpy(config.ssid, "ESP_Config", sizeof(config.ssid));
  config.ssid[sizeof(config.ssid) - 1] = '\0';

  strncpy(config.password, "11111111", sizeof(config.password));
  config.password[sizeof(config.password) - 1] = '\0';

  strncpy(config.lastColor, "blue", sizeof(config.lastColor));
  config.lastColor[sizeof(config.lastColor) - 1] = '\0';

  config.connected = false;
  config.bootCount = 0;
  config.brightness = 100;

  return config;
}

void EEPROMHandler::loadConfig(EEPROMConfig& config) {
  Serial.println("📥 Загрузка конфигурации из EEPROM...");

  Serial.println("✅ Конфигурация загружена успешно");
  if (strlen(config.ssid) > 0 && config.ssid[0] != 0xFF) {  // первый запуск
    Serial.println("⚠️ Первый запуск, сохранение значений по умолчанию");
    for (int i = 0; i < 512; i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
    Serial.println("🧹 EEPROM очищена");
    EEPROMConfig defaultConfig = getDefaultConfig();
    if (saveConfig(defaultConfig)) {
      config = defaultConfig;
    }
  } else {
    EEPROM.get(CONFIG_ADDRESS, config);
    Serial.println("📊 Текущая конфигурация:");
    printConfig(config);
  }
}

bool EEPROMHandler::saveConfig(const EEPROMConfig& config) {
  Serial.println("💾 Сохранение конфигурации в EEPROM...");
  // Сохраняем в EEPROM
  EEPROM.put(CONFIG_ADDRESS, config);

  // Применяем изменения
  if (EEPROM.commit()) {
    Serial.println("✅ Конфигурация сохранена успешно");
    Serial.println("📊 Новая конфигурация:");

    printConfig(config);
    return true;
  } else {
    Serial.println("❌ Ошибка сохранения в EEPROM");
    return false;
  }
}

void EEPROMHandler::printConfig(const EEPROMConfig& config) {
  if (!config.connected) {
    Serial.println("  ✅ Wi-Fi подключение для настроек:");
    Serial.print("  📶 SSID: ");
    Serial.println(config.ssidAP);
    Serial.print("  🔑 Пароль для отладки: ");
    Serial.println(config.passwordAP);
  } else {
    Serial.println("  ✅ Wi-Fi подключение:");
    Serial.printf("  📶 SSID:  ");
    Serial.println(config.ssid);
  }
  Serial.printf("  🎨 Последний цвет: ");
  Serial.println(config.lastColor);
  Serial.printf("  🔄 Счетчик перезагрузок: %u\n", config.bootCount);
  Serial.printf("  💡 Яркость: %d\n", config.brightness);
  Serial.println();
}