# Link Vídeo:
https://youtu.be/KavJYvXXtiw


# Plataforma de Monitoramento de Consumo e Geração de Energia Sustentável

## Instruções para Replicar e Testar a Solução

### 1. Configuração no Wokwi

1. Abra o [Wokwi](https://wokwi.com/).
2. Crie um novo projeto clicando em "New Project".
3. No projeto, adicione um ESP32 e conecte os sensores de geração e consumo de energia aos pinos apropriados (pinos 34 e 35 no código).
4. Copie o código abaixo e cole-o no editor de código do Wokwi:

```cpp
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
```



### 2. Configuração no Node-RED

1. Abra o [Node-RED].
2. No painel do Node-RED, clique no menu no canto superior direito e selecione **Import**.
3. Escolha a opção **Clipboard** e cole o código JSON abaixo:

```json
[
    {
        "id": "aadf4ee636b8c7a4",
        "type": "tab",
        "label": "Fluxo 10",
        "disabled": false,
        "info": "",
        "env": []
    },
    {
        "id": "gauge_generation",
        "type": "ui_gauge",
        "z": "aadf4ee636b8c7a4",
        "name": "Geração (kWh)",
        "group": "group_dashboard",
        "order": 1,
        "width": 6,
        "height": 3,
        "gtype": "gage",
        "title": "Geração de Energia (kWh)",
        "label": "kWh",
        "format": "{{value}}",
        "min": 0,
        "max": 10,
        "colors": [
            "#00b500",
            "#e6e600",
            "#ca3838"
        ],
        "seg1": 3,
        "seg2": 7,
        "x": 520,
        "y": 180,
        "wires": []
    },
    {
        "id": "gauge_consumption",
        "type": "ui_gauge",
        "z": "aadf4ee636b8c7a4",
        "name": "Consumo (kWh)",
        "group": "group_dashboard",
        "order": 2,
        "width": 6,
        "height": 3,
        "gtype": "gage",
        "title": "Consumo de Energia (kWh)",
        "label": "kWh",
        "format": "{{value}}",
        "min": 0,
        "max": 10,
        "colors": [
            "#00b500",
            "#e6e600",
            "#ca3838"
        ],
        "seg1": 3,
        "seg2": 7,
        "x": 520,
        "y": 280,
        "wires": []
    },
    {
        "id": "chart_energy",
        "type": "ui_chart",
        "z": "aadf4ee636b8c7a4",
        "name": "Monitoramento de Energia",
        "group": "group_dashboard",
        "order": 3,
        "width": 12,
        "height": 4,
        "label": "Energia (Geração vs Consumo)",
        "chartType": "line",
        "legend": "true",
        "xformat": "HH:mm:ss",
        "interpolate": "linear",
        "nodata": "Aguardando dados...",
        "ymin": "0",
        "ymax": "10",
        "removeOlder": "1",
        "removeOlderPoints": "100",
        "removeOlderUnit": "hour",
        "cutout": 0,
        "useOneColor": false,
        "colors": [
            "#1f77b4",
            "#ff7f0e",
            "#2ca02c",
            "#d62728",
            "#9467bd",
            "#8c564b",
            "#e377c2",
            "#7f7f7f",
            "#bcbd22",
            "#17becf"
        ],
        "outputs": 1,
        "x": 520,
        "y": 380,
        "wires": [
            []
        ]
    },
    {
        "id": "mqtt_in_generation",
        "type": "mqtt in",
        "z": "aadf4ee636b8c7a4",
        "name": "MQTT Geração",
        "topic": "iot/energy/generation",
        "qos": "0",
        "datatype": "auto",
        "broker": "mqtt_broker",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 220,
        "y": 180,
        "wires": [
            [
                "gauge_generation",
                "chart_energy"
            ]
        ]
    },
    {
        "id": "mqtt_in_consumption",
        "type": "mqtt in",
        "z": "aadf4ee636b8c7a4",
        "name": "MQTT Consumo",
        "topic": "iot/energy/consumption",
        "qos": "0",
        "datatype": "auto",
        "broker": "mqtt_broker",
        "nl": false,
        "rap": true,
        "rh": 0,
        "inputs": 0,
        "x": 220,
        "y": 280,
        "wires": [
            [
                "gauge_consumption",
                "chart_energy"
            ]
        ]
    },
    {
        "id": "group_dashboard",
        "type": "ui_group",
        "name": "Monitoramento",
        "tab": "dashboard_tab",
        "order": 1,
        "disp": true,
        "width": "12",
        "collapse": false
    },
    {
        "id": "mqtt_broker",
        "type": "mqtt-broker",
        "name": "Broker EMQX",
        "broker": "broker.emqx.io",
        "port": "1883",
        "clientid": "node-red-client",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": "4",
        "keepalive": "60",
        "cleansession": true,
        "autoUnsubscribe": true,
        "birthTopic": "",
        "birthQos": "0",
        "birthPayload": "",
        "closeTopic": "",
        "closeQos": "0",
        "closePayload": "",
        "willTopic": "",
        "willQos": "0",
        "willPayload": ""
    },
    {
        "id": "dashboard_tab",
        "type": "ui_tab",
        "name": "Energia IoT",
        "icon": "dashboard",
        "order": 1,
        "disabled": false,
        "hidden": false
    }
]
