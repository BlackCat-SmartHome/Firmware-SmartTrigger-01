/*

Firmware Revisão 1.0
Data: 29/08/2018
Responsável: Raphael Sander de Souza Pereira

Observação:
  - Preencher os valores de variável antes de subir o código para o microcontrolador;
  - O pino está configurado no D3.

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* WIFI_SSID = " "; // Nome da rede wireless.
const char* WIFI_PASSWORD = " "; // Senha da rede wireless.
const char* mqttserver = " "; // MQTT Server.
const uint16_t mqttport = 5555; // Porta de conexão ao MQTT Server.
const char* mqttuser = " "; // Usuário de conexão ao MQTT Server, NULL para sem usuário.
const char* mqttpass = " "; // Senha de conexão ao MQTT Server, NULL para sem senha.
const char* mqttid = " "; // ID de identificação MQTT.

void initPins();
void initSerial();
void initWiFi();
void initMQTT();

WiFiClient CLIENT;
PubSubClient MQTT(CLIENT);

void setup(void) {
  initPins();
  initSerial();
  initWiFi();
  initMQTT();
}

void loop() {
  if (!MQTT.connected()) {
    reconnectMQTT();
  }
  reconnectWiFi();
  MQTT.loop();
}

void initPins() {
  pinMode(D3, OUTPUT);
}

void initSerial() {
  delay(300);
  Serial.begin(9600);
}

void initWiFi() {
  
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
      MQTT.subscribe(mqttid);
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

