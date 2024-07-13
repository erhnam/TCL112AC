#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Tcl.h>

#include <Arduino.h>
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

//#define DEBUG

IPAddress staticIP(192, 168, 0, 254);  // Dirección IP estática que deseas asignar
IPAddress gateway(192, 168, 0, 1);    // Puerta de enlace
IPAddress subnet(255, 255, 255, 0);   // Máscara de subred
IPAddress dns1(8, 8, 8, 8); // optional
IPAddress dns2(8, 8, 4, 4); // optional

const char* ntpServerName = "pool.ntp.org";
const int timeZone = 0; // Zona horaria de España (UTC para evitar la doble corrección)
const long utcOffsetInSeconds = timeZone * 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, utcOffsetInSeconds);

AsyncWebServer server(80);

// HTML web page to handle 4 input fields
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>

<head>
  <title>Aire Acondicionado</title>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>
  <script defer src="https://cdn.jsdelivr.net/npm/alpinejs@3.x.x/dist/cdn.min.js"></script>
</head>

<body class="w3-light-gray">
    <div class='w3-container w3-card-4'>
      <div class='w3-center'>
        <h2 class='w3-center w3-text-blue'>Hora: TIME_VALUE</h2>
        <div class='w3-center' x-data="{ status: 'STATUS_VALUE' }">
        <label>Dispositivo: </label>
        <template x-if="status === 'ON'">
          <span class='w3-text-green' x-text='status'></span>
        </template>
        <template x-if="status === 'OFF'">
          <span class='w3-text-red' x-text='status'></span>
        </template>
      </div>
        
      </div>
    </div>

    <div class='w3-container w3-margin-bottom'>
      <form action='/getManual' class='w3-container w3-card-4 w3-margin-top w3-margin-bottom'>         
        <div class='w3-center w3-margin-bottom'>
          <h2 class='w3-center w3-text-blue'>Manual</h2>
          <table class='w3-table w3-border'>
            <tr>
              <td><label>Activar:</label></td>
              <td><select name="enable0" id="enable0Select" value='ENABLE_VALUE0'>
                <option value="enable">ON</option>
                <option value="disable">OFF</option>
              </select></td>
            </tr>
            <tr>
              <td><label>Ventilador:</label></td>
              <td><select name="fan0" id="fan0Select" value='FAN_VALUE0'>
                <option value="0">Auto</option>
                <option value="1">Mínimo</option>
                <option value="2">Bajo</option>
                <option value="3">Medio</option>
                <option value="4">Alto</option>
              </select></td>
            </tr>
            <tr>
              <td><label>Temperatura:</label></td>
              <td><input name='temp0' class='w3-input w3-border' type='number' value='TEMP_VALUE0' style='width: 5em;' />
              </td>
            </tr>
          </table>

          <div class='w3-center>
              <div class='w3-center w3-margin-top w3-margin-bottom'>
                <input class='w3-button w3-round-large w3-blue w3-margin-top' type='submit' value='Guardar'></button>
              </div>
          </div>
        </div>
    </form>  
  </div>
  
  <div class='w3-container'>
    <form action='/get' class='w3-container w3-card-4 w3-margin-top'>

      <div class='w3-half'>
        <h2 class='w3-center w3-text-blue'>Programación 1</h2>
        <table class='w3-table w3-border'>
          <tr>
            <td><label>Activar:</label></td>
            <td><select name="enable1" id="enable1Select" value='ENABLE_VALUE1'>
              <option value="enable">Activado</option>
              <option value="disable">Desactivado</option>
            </select></td>
          </tr>
          <tr>
            <td><label>Ventilador:</label></td>
            <td><select name="fan1" id="fan1Select" value='FAN_VALUE1'>
              <option value="0">Auto</option>
              <option value="1">Mínimo</option>
              <option value="2">Bajo</option>
              <option value="3">Medio</option>
              <option value="4">Alto</option>
            </select></td>
          </tr>
          <tr>
            <td><label>Temperatura:</label></td>
            <td><input name='temp1' class='w3-input w3-border' type='number' value='TEMP_VALUE1' style='width: 5em;' />
            </td>
          </tr>
          <tr>
            <td><label>Hora Inicio:</label></td>
            <td><input name='start1' class='w3-input w3-border' type='time' min='00:00' value='START_VALUE1' max='23:59'
                style='width: 5em;' />
            </td>
          </tr>
          <tr>
            <td><label>Hora Fin:</label></td>
            <td><input name='end1' class='w3-input w3-border' type='time' min='00:00' max='23:59' value='END_VALUE1'
                style='width: 5em;' />
            </td>
          </tr>
        </table>

      </div>

      <div class="w3-half">
        <h2 class='w3-center w3-text-blue'>Programación 2</h2>
        <table class='w3-table w3-border'>
          <tr>
            <td><label>Activar:</label></td>
            <td><select name="enable2" id="enable2Select" value='ENABLE_VALUE2'>
              <option value="enable">Activado</option>
              <option value="disable">Desactivado</option>
            </select></td>
          </tr>
          <tr>
            <td><label>Ventilador:</label></td>
            <td><select name="fan2" id="fan2Select" value='FAN_VALUE2'>
              <option value="0">Auto</option>
              <option value="1">Mínimo</option>
              <option value="2">Bajo</option>
              <option value="3">Medio</option>
              <option value="4">Alto</option>
            </select></td>
          </tr>
          <tr>
            <td><label>Temperatura:</label></td>
            <td><input name='temp2' class='w3-input w3-border' type='number' value='TEMP_VALUE2' style='width: 5em;' />
            </td>
          </tr>
          <tr>
            <td><label>Hora Inicio:</label></td>
            <td><input name='start2' class='w3-input w3-border' type='time' min='00:00' value='START_VALUE2' max='23:59'
                style='width: 5em;' />
            </td>
          </tr>
          <tr>
            <td><label>Hora Fin:</label></td>
            <td><input name='end2' class='w3-input w3-border' type='time' min='00:00' max='23:59' value='END_VALUE2'
                style='width: 5em;' />
            </td>
          </tr>
        </table>
      </div>
      <div class='w3-center w3-margin-top w3-margin-bottom'>
        <input class='w3-button w3-round-large w3-blue w3-margin-top' type='submit' value='Guardar'></button>
      </div>

    </form>
  </div>

<script>
  var selectElement = document.getElementById('enable0Select');

  for (var i = 0; i < selectElement.options.length; i++) {
    if (selectElement.options[i].value === 'ENABLE_VALUE0') {
      selectElement.options[i].selected = true;
      break;
    }
  }

  selectElement = document.getElementById('enable1Select');

  for (var i = 0; i < selectElement.options.length; i++) {
    if (selectElement.options[i].value === 'ENABLE_VALUE1') {
      selectElement.options[i].selected = true;
      break;
    }
  }
  
  selectElement = document.getElementById('enable2Select');

  for (var i = 0; i < selectElement.options.length; i++) {
    if (selectElement.options[i].value === 'ENABLE_VALUE2') {
      selectElement.options[i].selected = true;
      break;
    }
  }

  selectElement = document.getElementById('fan0Select');

  for (var i = 0; i < selectElement.options.length; i++) {
    if (selectElement.options[i].value === 'FAN_VALUE0') {
      selectElement.options[i].selected = true;
      break;
    }
  }
  
  selectElement = document.getElementById('fan1Select');

  for (var i = 0; i < selectElement.options.length; i++) {
    if (selectElement.options[i].value === 'FAN_VALUE1') {
      selectElement.options[i].selected = true;
      break;
    }
  }

  selectElement = document.getElementById('fan2Select');

  for (var i = 0; i < selectElement.options.length; i++) {
    if (selectElement.options[i].value === 'FAN_VALUE2') {
      selectElement.options[i].selected = true;
      break;
    }
  }

</script>

</body>

</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// Replace with your network credentials
const char* ssid     = "Sercomm2DE0";
const char* password = "HJYJMHF4FQMS8E";

const uint16_t kIrLed = D8;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRTcl112Ac ac(kIrLed);  // Set the GPIO to be used for sending messages.

const uint8_t num_programer = 2;

typedef struct manual {
  int temp;
  uint8_t fanMode;
  String enable;
  bool executed;
  bool old_executed;
} manual_t;

typedef struct programer {
  int temp;
  uint8_t fanMode;
  String startTime;
  String endTime;
  String enable;
  bool executed;
} programer_t;

manual_t manual = { 25.0, kTcl112AcFanMin, "disable", false};

/*
    case kTcl112AcAuto: 0
    case kTcl112AcCool: 1
    case kTcl112AcHeat: 2
    case kTcl112AcDry: 3
    case kTcl112AcFan: 4

    case kTcl112AcFanAuto: 0
    case kTcl112AcFanMin: 1
    case kTcl112AcFanLow: 2
    case kTcl112AcFanMed: 3
    case kTcl112AcFanHigh: 4
*/
programer_t programer[num_programer] = {
  {18.0, kTcl112AcFanMin, "21:30", "23:00", "disable", false},
  {25.0, kTcl112AcFanMin, "04:00", "04:30", "disable", false}
};

String formattedTime = "00:00";
bool power = false;
bool receive_get = false;

void setupFota(void)
{
 // Configuración de ArduinoOTA
  ArduinoOTA.setHostname("AireAC");  // Nombre del dispositivo en la red
  ArduinoOTA.setPassword("PacoAC"); // Contraseña para las actualizaciones FOTA
  
  ArduinoOTA.onStart([]() {
#ifdef DEBUG
    Serial.println("Inicio de actualización FOTA...");
#endif    
  });
  
  ArduinoOTA.onEnd([]() {
#ifdef DEBUG
    Serial.println("\nActualización FOTA completada.");
#endif
});
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef DEBUG
    Serial.printf("Progreso: %u%%\r", (progress / (total / 100)));
#endif
});
  
  ArduinoOTA.onError([](ota_error_t error) {
#ifdef DEBUG
    Serial.printf("Error [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Fallo de autenticación");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Error al iniciar la actualización FOTA");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Fallo de conexión");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Error en la recepción");
    else if (error == OTA_END_ERROR) Serial.println("Error al finalizar la actualización FOTA");
#endif
});

  ArduinoOTA.begin();  
}

void setupServer(void)
{
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String modifiedHtml = String(index_html);
    
    modifiedHtml.replace("TIME_VALUE", formattedTime);
    modifiedHtml.replace("STATUS_VALUE", power == true ? "ON" : "OFF");

    modifiedHtml.replace("FAN_VALUE0", String(manual.fanMode));
    modifiedHtml.replace("TEMP_VALUE0", String(manual.temp));
    modifiedHtml.replace("ENABLE_VALUE0", manual.enable);

    for (uint8_t i = 0; i < num_programer; i++) {
      modifiedHtml.replace("ENABLE_VALUE" + String(i+1), programer[i].enable);
      modifiedHtml.replace("FAN_VALUE" + String(i+1), String(programer[i].fanMode));
      modifiedHtml.replace("TEMP_VALUE" + String(i+1), String(programer[i].temp));
      modifiedHtml.replace("START_VALUE" + String(i+1), programer[i].startTime);
      modifiedHtml.replace("END_VALUE" + String(i+1), programer[i].endTime);  
    }

    request->send(200, "text/html", modifiedHtml.c_str());
  });

  // Send a GET request Programacion
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest * request) {
    receive_get = true;
    for (uint8_t i = 0; i < num_programer; i++) {
      programer[i].enable = request->getParam("enable"+String(i+1))->value();
      programer[i].fanMode = request->getParam("fan"+String(i+1))->value().toInt();
      programer[i].temp = request->getParam("temp" + String(i + 1))->value().toInt();
      programer[i].startTime = request->getParam("start"+String(i+1))->value();
      programer[i].endTime = request->getParam("end"+String(i+1))->value();
      programer[i].executed = false;
    }

    ac.setPower(false);

#ifdef DEBUG
    for (uint8_t i = 0; i < num_programer; i++) {
      Serial.println("Enable " + String(i+1) + ": " + programer[i].enable);
      Serial.println("Fan " + String(i+1) + ": " + programer[i].fanMode);
      Serial.println("Temp " + String(i+1) + ": " + String(programer[i].temp));
      Serial.println("Start " + String(i+1) + ": " + programer[i].startTime);
      Serial.println("ends " + String(i+1) + ": " +  programer[i].endTime);
    }
#endif

#ifdef DEBUG  
    Serial.println("Status: " + power);
#endif

    String modifiedHtml = String(index_html);

    modifiedHtml.replace("TIME_VALUE", formattedTime);

    modifiedHtml.replace("STATUS_VALUE", power == true ? "ON" : "OFF");

    modifiedHtml.replace("FAN_VALUE0", String(manual.fanMode));
    modifiedHtml.replace("TEMP_VALUE0", String(manual.temp));
    modifiedHtml.replace("ENABLE_VALUE0", manual.enable);
    
    for (uint8_t i = 0; i < num_programer; i++) {
      modifiedHtml.replace("ENABLE_VALUE" + String(i+1), programer[i].enable);
      modifiedHtml.replace("FAN_VALUE" + String(i+1), String(programer[i].fanMode));
      modifiedHtml.replace("TEMP_VALUE" + String(i+1), String(programer[i].temp));
      modifiedHtml.replace("START_VALUE" + String(i+1), programer[i].startTime);
      modifiedHtml.replace("END_VALUE" + String(i+1), programer[i].endTime);
    }

    request->send(200, "text/html", modifiedHtml.c_str());
  });

  // Send a GET request Manual
  server.on("/getManual", HTTP_GET, [](AsyncWebServerRequest * request) {
    receive_get = true;
    manual.temp = request->getParam("temp0")->value().toInt();
    manual.fanMode = request->getParam("fan0")->value().toInt();
    manual.enable = request->getParam("enable0")->value();
    manual.old_executed = manual.executed;
    manual.executed = false;

#ifdef DEBUG
    Serial.println("Enable 0: " + manual.enable);
    Serial.println("Fan 0: "  + manual.fanMode);
    Serial.println("Temp 0: " + String(manual.temp));
#endif

#ifdef DEBUG  
    Serial.println("Status: " + power);
#endif

    String modifiedHtml = String(index_html);

    modifiedHtml.replace("TIME_VALUE", formattedTime);

    modifiedHtml.replace("STATUS_VALUE", power == true ? "ON" : "OFF");

    modifiedHtml.replace("FAN_VALUE0", String(manual.fanMode));
    modifiedHtml.replace("TEMP_VALUE0", String(manual.temp));
    modifiedHtml.replace("ENABLE_VALUE0", manual.enable);

    for (uint8_t i = 0; i < num_programer; i++) {
      modifiedHtml.replace("ENABLE_VALUE" + String(i+1), programer[i].enable);
      modifiedHtml.replace("FAN_VALUE" + String(i+1), String(programer[i].fanMode));
      modifiedHtml.replace("TEMP_VALUE" + String(i+1), String(programer[i].temp));
      modifiedHtml.replace("START_VALUE" + String(i+1), programer[i].startTime);
      modifiedHtml.replace("END_VALUE" + String(i+1), programer[i].endTime);
    }
    
    request->send(200, "text/html", modifiedHtml.c_str());

  });

  server.onNotFound(notFound);
  server.begin();
}

void connectToWiFi(void)
{
  WiFi.config(staticIP, gateway, subnet, dns1, dns2);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

#ifdef DEBUG
  Serial.println("Connecting to WiFi...");
#endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
#ifdef DEBUG
    Serial.println("WiFi Connection Failed. Reconnecting...");
#endif
  }

#ifdef DEBUG
  Serial.println("Connected to WiFi.");
  Serial.println("IP Address: " + WiFi.localIP().toString());
#endif
}


void formatHour(void)
{
  // Formatea la hora y los minutos en una cadena "HH:MM"
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  formattedTime = "";
  // Formatea la hora y los minutos en una cadena "HH:MM"
  if (currentHour < 10) {
     formattedTime += "0"; // Agrega un 0 inicial si las horas son menores a 10
  }
  
  formattedTime += String(currentHour);
  formattedTime += ":";
  
  if (currentMinute < 10) {
    formattedTime += "0"; // Agrega un 0 inicial si los minutos son menores a 10
  }

  formattedTime += String(currentMinute);  
}

void setup() {
  ac.begin();
  ac.setPower(false);
  power = ac.getPower();

#ifdef DEBUG  
  Serial.begin(115200);
#endif
  
  connectToWiFi();

  // Initialize a NTPClient to get time
  timeClient.begin();
  
  // España
  timeClient.setTimeOffset(7200);

  setupFota();
  
  setupServer();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }
  
  ArduinoOTA.handle();

  if (receive_get) {
    receive_get = false;
    programer[0].executed = false; 
    programer[1].executed = false;
    ac.setPower(false);
    ac.send();
#ifdef DEBUG
    Serial.println("Encendido Manual");
#endif
  }

  timeClient.update();
  
  formatHour();

  // Encendido Manual
  if (manual.enable == "enable" && manual.executed == false) {
#ifdef DEBUG
    Serial.println("Encendido Manual");
#endif
    ac.setPower(true);
    ac.setMode(kTcl112AcCool);
    ac.setTemp(manual.temp);
    ac.setFan(manual.fanMode);
    ac.send();
    manual.executed = true;
  } 

  // Apagado Manual
  if (manual.enable == "disable" && manual.old_executed == true) {
#ifdef DEBUG
    Serial.println("Apagado Manual");
#endif
    manual.executed = false;
    manual.old_executed = false;
    ac.setPower(false);
    ac.send();
  }
  
  for (uint8_t i = 0; i < num_programer; i++) {
    if (programer[i].enable == "enable") {      
      // Encendido
      if (formattedTime == programer[i].startTime && programer[i].executed == false) {
#ifdef DEBUG
        Serial.println("Programacion " + String(i+1) + "Activada");
#endif
        ac.setPower(true);
        ac.setMode(kTcl112AcCool);
        ac.setTemp(programer[i].temp);
        ac.setFan(programer[i].fanMode);
        ac.setLight(false);
        ac.send();
        programer[i].executed = true;
      }
  
      // Apagado
      if (formattedTime == programer[i].endTime && programer[i].executed == true) {
#ifdef DEBUG
       Serial.println("Programacion " + String(i+1) + "Desactivada");
#endif      
        ac.setPower(false);
        ac.send();
        programer[i].executed = false;
      }
    }  
  }
  
  delay(1000);
}
