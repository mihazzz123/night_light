#include "led_control.h"

void handleLedPage()
{
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'>"
                "<title>NeoPixel Control</title></head><body>"
                "<h2>Управление NeoPixel</h2>"
                "<form action='/setColor' method='GET'>"
                "R: <input type='range' name='r' min='0' max='255'><br>"
                "G: <input type='range' name='g' min='0' max='255'><br>"
                "B: <input type='range' name='b' min='0' max='255'><br><br>"
                "<input type='submit' value='Установить цвет'>"
                "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSetColor()
{
  if (!server.hasArg("r") || !server.hasArg("g") || !server.hasArg("b"))
  {
    server.send(400, "text/plain", "Missing parameters");
    return;
  }

  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();

  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();

  server.sendHeader("Location", "/led");
  server.send(303); // Перенаправление обратно на страницу управления
}
