#include <WiFi.h>
#include <PubSubClient.h>

// Credenciais WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configuração do broker MQTT
const char* mqttServer = "broker.emqx.io"; // Novo broker
const int mqttPort = 1883;

// Tópicos MQTT
const char* generationTopic = "iot/energy/generation";
const char* consumptionTopic = "iot/energy/consumption";

// Objetos WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis simuladas
float energyGenerated = 0.0;
float energyConsumed = 0.0;

// Pinos dos dispositivos
const int generationSensorPin = 34;  // Simula geração de energia
const int consumptionSensorPin = 35; // Simula consumo de energia

void setup() {
  Serial.begin(115200);

  // Conexão WiFi
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado ao WiFi!");

  // Configuração MQTT
  client.setServer(mqttServer, mqttPort);

  // Conecta ao broker
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Simulação das leituras
  energyGenerated = analogRead(generationSensorPin) / 4095.0 * 10.0; // 0-10 kWh
  energyConsumed = analogRead(consumptionSensorPin) / 4095.0 * 10.0; // 0-10 kWh

  // Publicação dos dados
  publishData();

  delay(5000); // Intervalo de publicação
}

void publishData() {
  char generatedStr[10];
  char consumedStr[10];

  // Converte valores float para string
  dtostrf(energyGenerated, 4, 2, generatedStr);
  dtostrf(energyConsumed, 4, 2, consumedStr);

  // Publica geração
  client.publish(generationTopic, generatedStr);

  // Publica consumo
  client.publish(consumptionTopic, consumedStr);

  Serial.println("Dados publicados:");
  Serial.print("Geração: ");
  Serial.println(generatedStr);
  Serial.print("Consumo: ");
  Serial.println(consumedStr);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" Conectado!");
    } else {
      Serial.print(" Falha (rc=");
      Serial.print(client.state());
      Serial.println("). Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}
