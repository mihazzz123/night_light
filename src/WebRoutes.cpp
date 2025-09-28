#include "WebRoutes.h"

#include "Config.h"
#include "Helper.h"
#include "LEDHandler.h"
#include "WiFiManager.h"

ESP8266WebServer* WebRoutes::_server = nullptr;
Adafruit_NeoPixel* WebRoutes::_strip = nullptr;

void WebRoutes::registerRoutes(ESP8266WebServer* server,
                               Adafruit_NeoPixel* strip) {
  _server = server;
  _strip = strip;

  server->on("/", handleRoot);
  server->on("/setColor", handleSetColor);
  server->on("/updateWiFi", HTTP_POST, handleUpdateWiFi);
  server->on("/restart", handleRestart);
  server->on("/connectWiFi", HTTP_POST, handleConnectWiFi);

  // Captive Portal
  server->on("/generate_204", handleRoot);         // Android
  server->on("/redirect", handleRoot);             // Windows
  server->on("/hotspot-detect.html", handleRoot);  // macOS
  server->on("/ncsi.txt", handleRoot);             // Windows
  server->on("/connecttest.txt", handleRoot);      // Windows
  server->on("/captiveportal", handleRoot);        // fallback
  server->on("/scanNetworks", handleScanNetworks);

  server->onNotFound([server]() {
    server->sendHeader("Location", "/", true);
    server->send(302, "text/plain", "");
  });
}

void WebRoutes::handleRoot() {
  String html =
      "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Управление "
      "светодиодами</title>";
  html += "<style>body { font-family: Arial; margin: 40px; }";
  html += ".btn { padding: 10px 20px; margin: 5px; font-size: 16px; }";
  html += ".color-picker { margin: 10px 0; }</style></head><body>";

  html += "<h1>🎮 Управление светодиодами</h1>";

  // 📊 Информация о подключении
  html +=
      "<div style='background: #f0f0f0; padding: 15px; border-radius: 5px; "
      "margin-bottom: 20px;'>";
  html += "<h3>📡 Состояние сети:</h3>";

  if (WiFi.status() == WL_CONNECTED) {
    html += "<p style='color: green;'>✅ Подключено к Wi-Fi</p>";
    html +=
        "<p><strong>SSID:</strong> " + escapeHtml(String(WiFi.SSID())) + "</p>";
    html +=
        "<p><strong>IP адрес:</strong> " + WiFi.localIP().toString() + "</p>";
    html += "<p><strong>Уровень сигнала:</strong> " +
            escapeHtml(String(WiFi.RSSI())) + " dBm</p>";
  } else {
    html += "<p style='color: red;'>❌ Не подключено к Wi-Fi</p>";
    html += "<form action='/connectWiFi' method='POST'>";
    html += "<p>SSID: <select name='ssid'>";

    int n = WiFi.scanComplete();
    if (n == -2)
      WiFi.scanNetworks(true);  // если сканирование не запущено
    else if (n > 0) {
      for (int i = 0; i < n; ++i) {
        html += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) +
                "</option>";
      }
      WiFi.scanDelete();  // очистка результатов
    }

    html += "</select></p>";
    html += "<p>Пароль: <input type='password' name='password'></p>";
    html += "<button type='submit' class='btn'>🔌 Подключиться</button>";
    html += "</form>";
  }
  html += "</div>";

  // 🎨 Палитра цветов
  html += "<h3>🎨 Выбор цвета:</h3>";
  html += "<div class='color-picker'>";
  html +=
      "<button class='btn' style='background:red; color:white;' "
      "onclick=\"setColor('red')\">🔴 Красный</button>";
  html +=
      "<button class='btn' style='background:green; color:white;' "
      "onclick=\"setColor('green')\">🟢 Зеленый</button>";
  html +=
      "<button class='btn' style='background:blue; color:white;' "
      "onclick=\"setColor('blue')\">🔵 Синий</button>";
  html +=
      "<button class='btn' style='background:yellow;' "
      "onclick=\"setColor('yellow')\">🟡 Желтый</button>";
  html +=
      "<button class='btn' style='background:purple; color:white;' "
      "onclick=\"setColor('purple')\">🟣 Фиолетовый</button>";
  html +=
      "<button class='btn' style='background:white;' "
      "onclick=\"setColor('white')\">⚪ Белый</button>";
  html +=
      "<button class='btn' style='background:black; color:white;' "
      "onclick=\"setColor('black')\">⚫ Выключить</button>";
  html += "</div>";

  // ⚙️ Настройка Wi-Fi
  html += "<h3>⚙️ Настройка Wi-Fi:</h3>";
  html += "<form action='/updateWiFi' method='POST'>";
  html += "<p>SSID: <input type='text' name='ssid' value='" +
          escapeHtml(String(config.ssid)) + "'></p>";
  html += "<p>Пароль: <input type='password' name='password' value='" +
          escapeHtml(String(config.password)) + "'></p>";
  html += "<button type='submit' class='btn'>💾 Сохранить настройки</button>";
  html += "</form>";

  // 🔄 Перезагрузка
  html += "<h3>🔄 Управление:</h3>";
  html +=
      "<button class='btn' onclick=\"fetch('/restart')\">🔄 "
      "Перезагрузить</button>";

  // 📜 JavaScript для AJAX запросов
  html += "<script>";
  html += "function setColor(color) {";
  html +=
      "  fetch('/setColor?color=' + color).then(r => console.log('Цвет "
      "изменен: ' + color));";
  html += "}";

  html += "function updateSSIDList() {";
  html += "  fetch('/scanNetworks')";
  html += "    .then(response => response.json())";
  html += "    .then(ssids => {";
  html +=
      "      const select = document.querySelector(\"select[name='ssid']\");";
  html += "      select.innerHTML = \"\";";
  html += "      ssids.forEach(ssid => {";
  html += "        const option = document.createElement(\"option\");";
  html += "        option.value = ssid;";
  html += "        option.textContent = ssid;";
  html += "        select.appendChild(option);";
  html += "      });";
  html += "    });";
  html += "}";

  html += "setInterval(updateSSIDList, 5000);";
  html += "updateSSIDList();";
  html += "</script>";

  html += "</body></html>";

  _server->send(200, "text/html", html);
}

void WebRoutes::handleSetColor() {
  String color = _server->arg("color");
  LEDHandler::setColor(_strip, color);
  _server->send(200, "text/plain", "Цвет изменен на: " + color);
}

void WebRoutes::handleUpdateWiFi() {
  if (!_server->hasArg("ssid") || !_server->hasArg("password")) {
    _server->send(400, "text/plain", "Ошибка: отсутствуют параметры");
    return;
  }
  String newSSID = _server->arg("ssid");
  String newPassword = _server->arg("password");

  // Сохраняем в EEPROM
  updateWiFiConfig(newSSID, newPassword);

  Serial.println("⚙️ Настройки Wi-Fi обновлены и сохранены в EEPROM:");
  Serial.println("SSID: " + newSSID);

  _server->send(200, "text/html",
                "<script>alert('Настройки сохранены в память!'); "
                "window.location='/';</script>");
}

void WebRoutes::handleRestart() {
  _server->send(200, "text/plain", "Перезагрузка...");
  delay(1000);
  ESP.restart();
}

void WebRoutes::handleConnectWiFi() {
  if (WiFiManager::handleConnectWiFi(_server)) {
    _server->send(200, "text/html",
                  "<script>alert('Подключено!'); location.href='/';</script>");
  } else {
    _server->send(
        200, "text/html",
        "<script>alert('Ошибка подключения'); location.href='/';</script>");
  }
}

void WebRoutes::handleScanNetworks() {
  int n = WiFi.scanComplete();
  if (n == -2) {
    WiFi.scanNetworks(true);  // запускаем сканирование
    _server->send(200, "application/json", "[]");
    return;
  }

  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0)
      json += ",";
    json += "\"" + WiFi.SSID(i) + "\"";
  }
  json += "]";
  WiFi.scanDelete();        // очистка
  WiFi.scanNetworks(true);  // запускаем новое сканирование
  _server->send(200, "application/json", json);
}
