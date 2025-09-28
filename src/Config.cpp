#include "Config.h"

EEPROMConfig config;
WState currentState = WIFI_DISCONNECTED;
bool ready = false;

void loadAppConfig() {
  EEPROMHandler::loadConfig(config);
}

void saveAppConfig() {
  EEPROMHandler::saveConfig(config);
}

void updateWiFiConfig(const String& ssid, const String& password) {
  strncpy(config.ssid, ssid.c_str(), sizeof(config.ssid));
  config.ssid[sizeof(config.ssid) - 1] = '\0';

  strncpy(config.password, password.c_str(), sizeof(config.password));
  config.password[sizeof(config.password) - 1] = '\0';

  saveAppConfig();
}

void updateColorConfig(const String& color) {
  strncpy(config.lastColor, color.c_str(), sizeof(config.lastColor));
  config.lastColor[sizeof(config.lastColor) - 1] = '\0';

  saveAppConfig();
}