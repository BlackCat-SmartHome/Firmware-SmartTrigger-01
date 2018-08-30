#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <FS.h>

/*


Wireless AP
Wireless STA
MQTT
Arquivo de configuração => https://github.com/uagaviria/ESP8266---SPIFFS---JSON
Botão de reset
Requests
Web Server
mDNS => https://tttapa.github.io/ESP8266/Chap08%20-%20mDNS.html

Para verificar:
String mqttClientId = String("ESP8266Client-") + ESP.getChipId();


*/

const char* WIFI_SSID = "uaifai";
const char* WIFI_PASSWORD = "123rep456";

const char* mqttserver = "m12.cloudmqtt.com"; //m12.cloudmqtt.com (CloudMQTT)
const uint16_t mqttport = 12059; //12059 (CloudMQTT)
const char* mqttuser = "mwxjdcnp"; //NULL para sem usuário //mwxjdcnp (CloudMQTT)
const char* mqttpass = "cmrKrMLr--Rj"; //NULL para sem senha //cmrKrMLr--Rj (CloudMQTT)
const char* mqttid = "67482172314";

const int AP_CLIENTE = D5;
const int BOT = D0;

int lastButtonState = 0;
int buttonState = 0;

int statusCode;

void initPins();
void initSerial();
void initWiFi();
void initMQTT();
void initSPIFFS();
void initResetJumper();
void initServer();

WiFiClient CLIENT;

PubSubClient MQTT(CLIENT);

ESP8266WebServer server(80);
#include "data.h"

void setup(void) {
  initPins();
  initSerial();
  initWiFi();
  initMQTT();
  initServer();
}

void loop() {
  //if (!MQTT.connected()) {
    //reconnectMQTT();
  //}
  //reconnectWiFi();
  BotState();
  MQTT.loop();
  server.handleClient();
}

void initServer() {
  // HTML
  server.on ("/", indexHTML);
  server.on ("/settings.html", settingsHTML);
  server.on ("/apscan.html", apscanHTML);
  server.on ("/info.html", infoHTML);
  server.on ("/error.html", errorHTML);

  // JS
  server.on("/js/functions.js", loadFunctionsJS);
  server.on("/js/settings.js", loadSettingsJS);

  // JSON
  server.on("/config", saveSettings);

  // CSS
  server.on ("/style.css", loadStyle);

  server.begin();
}

void apscanHTML() {
  sendFile(200, "text/html", data_apscanHTML, sizeof(data_apscanHTML));
}

void settingsHTML() {
  sendFile(200, "text/html", data_settingsHTML, sizeof(data_settingsHTML));
}

void errorHTML() {
  sendFile(200, "text/html", data_errorHTML, sizeof(data_errorHTML));
}

void indexHTML() {
  sendFile(200, "text/html", data_indexHTML, sizeof(data_indexHTML));
}

void infoHTML() {
  sendFile(200, "text/html", data_infoHTML, sizeof(data_infoHTML));
}

void loadFunctionsJS() {
  sendFile(200, "text/javascript", data_js_functionsJS, sizeof(data_js_functionsJS));
}

void loadSettingsJS() {
  sendFile(200, "text/javascript", data_js_settingsJS, sizeof(data_js_settingsJS));
}

void loadStyle() {
  sendFile(200, "text/css;charset=UTF-8", data_styleCSS, sizeof(data_styleCSS));
}

void saveSettings() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  server.send(200, "text/plain", "true");
}

void BotState() {
  if (digitalRead(D0) != lastButtonState) {
    if (lastButtonState == 0) {
      digitalWrite(D3, 1);
    }
    if (lastButtonState == 1) {
      digitalWrite(D3, 0);
    }
  }

  lastButtonState = digitalRead(D0);
}

bool loadConfig() {
  bool result = SPIFFS.begin();
  Serial.print("SPIFFS... ");
  if (!result) {
    Serial.println("[OK]");
  } else {
    Serial.println("[ERRO]");
	return false;
  }
  File configfile = SPIFFS.open("/config.json", "r");
  
  if (!configfile) {
    Serial.print("Criando arquivo config... ");
    File configfile = SPIFFS.open("/config.json", "w");
    if (!configfile) {
      Serial.println("[ERRO]");
	  return false;
    } else {
      Serial.println("[OK]");
    }
    configfile.print("{mqttserver: NULL, mqttport: NULL, mqttid: NULL, mqttuser: NULL, mqttpass: NULL, ssid: NULL, pass: NULL}");
  } else {
    while(configfile.available()) {
      Teste += configfile.readStringUntil('n');
    }
  }
  
  size_t size = configFile.size();
  
  Serial.print("Tamanho do arquivo... ")
  if (size > 1024) {
    Serial.println("[ERRO]");
    return false;
  } else {
	Serial.print("[OK]")
  }
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());
	
  Serial.print("Verificando JSON... ")
  if (!json.success()) {
    Serial.println("[ERRO]");
    return false;
  } else {
	Serial.println("[OK]");
  }
  
  const char* WIFI_SSID = json["ssid"];
  const char* WIFI_PASSWORD = json["pass"];
  
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("PASSWORD: ");
  Serial.println(pass);
  
  configfile.close();
  
  return true;
}

void saveConfig() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["serverName"] = "api.example.com";
  json["accessToken"] = "128du9as8du12eoue8da98h123ueh9h98";

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Error al abrir el archivo de configuración para escribir");
    return false;
  }

  json.printTo(configFile);
  return true;
}


void initPins() {
  pinMode(D3, OUTPUT);
  pinMode(D5, INPUT);
  pinMode(D0, INPUT_PULLUP);
}

void initSerial() {
  delay(300);
  Serial.begin(9600);
}

void initWiFi() {
  Serial.print("Configurando AP... ");
  boolean result = WiFi.softAP("AutoConnect", "00000000");
  if(result == true) {
    Serial.println("[OK]");
  } else {
    Serial.println("[ERRO]");
  }
}


void WIFI() {
  
  delay(10);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Conectando na rede ");
  Serial.println(WIFI_SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(1000);
  }
  
  Serial.println();
  Serial.println("Conectado na Rede " + String(WIFI_SSID));
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(mqttserver, mqttport);
  MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.println("Conectando ao Broker MQTT");
    if (MQTT.connect("ESP8266", mqttuser, mqttpass)) {
      Serial.println("Conectado com Sucesso ao Broker");
      MQTT.subscribe("blackcat/cont");
    } else {
      Serial.print("Falha ao Conectador, Código= ");
      Serial.println(MQTT.state());
      delay(2000);
    }
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  String message;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    message += c;
  }
  Serial.println("Tópico => " + String(topic) + " | Valor => " + String(message));
  if (message == "ON") {
    digitalWrite(D3, 1);
  } else {
    digitalWrite(D3, 0);
  }
  Serial.flush();
}

void reconnectWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
}
