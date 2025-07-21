// Incluye tus secretos WiFi
#include "secrets.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Tcl.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Constantes generales
const char* ntpServerName = "pool.ntp.org";
const int timeZone = 0; // UTC para evitar errores de DST
const long utcOffsetInSeconds = timeZone * 3600;

const char* ENABLED = "enable";
const char* DISABLED = "disable";

// Red
IPAddress staticIP(192, 168, 0, 254);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(8, 8, 8, 8);
IPAddress dns2(8, 8, 4, 4);

// Web y NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, utcOffsetInSeconds);
AsyncWebServer server(80);

// Aire Acondicionado
const uint16_t kIrLed = D8;
IRTcl112Ac ac(kIrLed);
bool power = false;
bool receive_get = false;

// Estructuras de configuración
const uint8_t num_programer = 2;

typedef struct manual {
  int temp;
  uint8_t fanMode;
  String enable;
  String mute;
  bool executed;
  bool old_executed;
} manual_t;

typedef struct programer {
  int temp;
  uint8_t fanMode;
  String startTime;
  String endTime;
  String enable;
  String mute;
  bool executed;
} programer_t;

manual_t manual = {25, kTcl112AcFanMin, DISABLED, DISABLED, false, false};
programer_t programer[num_programer] = {
  {18, kTcl112AcFanMin, "21:30", "23:00", DISABLED, DISABLED, false},
  {25, kTcl112AcFanMin, "04:00", "04:30", DISABLED, DISABLED, false}
};

String formattedTime = "00:00";

// Página HTML cargada desde PROGMEM
#include "html_page.h"

// Función para renderizar la página con valores dinámicos
String renderHTML() {
  String html = String(index_html);
  html.replace("TIME_VALUE", formattedTime);
  html.replace("STATUS_VALUE", power ? "ON" : "OFF");
  html.replace("FAN_VALUE0", String(manual.fanMode));
  html.replace("TEMP_VALUE0", String(manual.temp));
  html.replace("ENABLE_VALUE0", manual.enable);
  html.replace("MUTE_VALUE0", manual.mute);

  for (uint8_t i = 0; i < num_programer; i++) {
    html.replace("ENABLE_VALUE" + String(i + 1), programer[i].enable);
    html.replace("MUTE_VALUE" + String(i + 1), programer[i].mute);
    html.replace("FAN_VALUE" + String(i + 1), String(programer[i].fanMode));
    html.replace("TEMP_VALUE" + String(i + 1), String(programer[i].temp));
    html.replace("START_VALUE" + String(i + 1), programer[i].startTime);
    html.replace("END_VALUE" + String(i + 1), programer[i].endTime);
  }
  return html;
}

void sendAC(bool on, int temp, int fanMode, bool mute) {
  ac.setPower(on);
  ac.setMode(kTcl112AcCool);
  ac.setTemp(temp);
  ac.setFan(fanMode);
  ac.setQuiet(mute);
  ac.setLight(false);
  ac.send();
  power = on;
}

void formatHour() {
  char buffer[6];
  sprintf(buffer, "%02d:%02d", timeClient.getHours(), timeClient.getMinutes());
  formattedTime = String(buffer);
}

void setupFota() {
  ArduinoOTA.setHostname("AireAC");
  ArduinoOTA.setPassword("PacoAC");
  ArduinoOTA.onStart([]() { });
  ArduinoOTA.onEnd([]() { });
  ArduinoOTA.onProgress([](unsigned int, unsigned int) { });
  ArduinoOTA.onError([](ota_error_t) { });
  ArduinoOTA.begin();
}

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", renderHTML());
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    request->send(response);
  });

  server.on("/getManual", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("temp0")) manual.temp = request->getParam("temp0")->value().toInt();
    if (request->hasParam("fan0")) manual.fanMode = request->getParam("fan0")->value().toInt();
    if (request->hasParam("enable0")) manual.enable = request->getParam("enable0")->value();
    if (request->hasParam("mute0")) manual.mute = request->getParam("mute0")->value();

    manual.old_executed = manual.executed;
    manual.executed = false;
    receive_get = true;

      // Redirige a la raíz
    request->redirect("/");
  });

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    for (uint8_t i = 0; i < num_programer; i++) {
      if (request->hasParam("enable" + String(i + 1)))
        programer[i].enable = request->getParam("enable" + String(i + 1))->value();
      if (request->hasParam("mute" + String(i + 1)))
        programer[i].mute = request->getParam("mute" + String(i + 1))->value();
      if (request->hasParam("fan" + String(i + 1)))
        programer[i].fanMode = request->getParam("fan" + String(i + 1))->value().toInt();
      if (request->hasParam("temp" + String(i + 1)))
        programer[i].temp = request->getParam("temp" + String(i + 1))->value().toInt();
      if (request->hasParam("start" + String(i + 1)))
        programer[i].startTime = request->getParam("start" + String(i + 1))->value();
      if (request->hasParam("end" + String(i + 1)))
        programer[i].endTime = request->getParam("end" + String(i + 1))->value();
      programer[i].executed = false;
    }
    receive_get = true;

    // Redirige a la raíz
    request->redirect("/");
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
}

void connectToWiFi() {
  WiFi.config(staticIP, gateway, subnet, dns1, dns2);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
}

void setup() {
  ac.begin();
  ac.setPower(false);
  power = false;

  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  connectToWiFi();
  timeClient.begin();
  timeClient.setTimeOffset(7200); // España
  setupFota();
  setupServer();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
  ArduinoOTA.handle();
  timeClient.update();
  formatHour();

  if (receive_get) {
    receive_get = false;
    for (uint8_t i = 0; i < num_programer; i++) programer[i].executed = false;
    ac.setPower(false);
    ac.send();
  }

  if (manual.enable == ENABLED && !manual.executed) {
    sendAC(true, manual.temp, manual.fanMode, manual.mute == ENABLED);
    manual.executed = true;
  } else if (manual.enable == DISABLED && manual.old_executed) {
    sendAC(false, 0, 0, false);
    manual.executed = false;
    manual.old_executed = false;
  }

  for (uint8_t i = 0; i < num_programer; i++) {
    if (programer[i].enable == ENABLED) {
      if (formattedTime == programer[i].startTime && !programer[i].executed) {
        sendAC(true, programer[i].temp, programer[i].fanMode, programer[i].mute == ENABLED);
        programer[i].executed = true;
      } else if (formattedTime == programer[i].endTime && programer[i].executed) {
        sendAC(false, 0, 0, false);
        programer[i].executed = false;
      }
    }
  }

  delay(1000);
}
