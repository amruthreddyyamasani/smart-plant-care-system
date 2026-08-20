#include <WiFi.h>
#include <PubSubClient.h>
#include "DHTesp.h"

// Pin Definitions
const int DHT_PIN = 15;
const int LDR_PIN = 34;
const int SOIL_PIN = 35;

// WiFi & MQTT Configuration
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* MQTT_SERVER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "plantCare/status/data";

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dhtSensor;

void setupWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    String clientId = "ESP32PlantClient-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      break;
    }
    delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  setupWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Read Sensors
  TempAndHumidity dhtData = dhtSensor.getTempAndHumidity();
  
  // Safety check for DHT22 read errors
  if (dhtSensor.getStatus() != DHTesp::ERROR_NONE) {
    Serial.println("Error reading DHT22 sensor!");
    delay(2000);
    return;
  }

  float temp = dhtData.temperature;
  float humidity = dhtData.humidity;
  int rawLight = analogRead(LDR_PIN);
  int rawSoil = analogRead(SOIL_PIN);

  // Convert analog readings (0-4095) to percentages (0-100%)
  float lightPercent = map(rawLight, 0, 4095, 0, 100);
  float soilPercent = map(rawSoil, 0, 4095, 0, 100);

  // Default Status
  String plantStatus = "Healthy";

  // 1. Single-Sensor Base Conditions
  if (soilPercent < 30.0) {
    plantStatus = "Needs Water";
  } else if (lightPercent < 25.0) {
    plantStatus = "Needs Light";
  } else if (temp > 35.0) {
    plantStatus = "Too Hot";
  }

  // 2. Custom Multi-Sensor Conditions (Higher Priority Checks)

  // Custom Condition 1: Fungal & Root Rot Risk
  if (soilPercent > 80.0 && humidity > 85.0) {
    plantStatus = "Fungal & Rot Risk";
  } 
  // Custom Condition 2: Frost & Freeze Risk
  else if (temp < 10.0 && humidity > 80.0) {
    plantStatus = "Frost & Freeze Risk";
  } 
  // Custom Condition 3: Sunburn & Scorch Danger
  else if (lightPercent > 80.0 && temp > 35.0) {
    plantStatus = "Sunburn & Scorch Danger";
  }

  // Construct JSON Payload
  String payload = "{\"temp\":" + String(temp, 1) + 
                   ",\"humidity\":" + String(humidity, 1) + 
                   ",\"light\":" + String(lightPercent, 0) + 
                   ",\"soil\":" + String(soilPercent, 0) + 
                   ",\"status\":\"" + plantStatus + "\"}";

  Serial.println("Publishing: " + payload);
  client.publish(MQTT_TOPIC, payload.c_str());

  delay(5000); // Wait 5 seconds before next read cycle
}
Add sketch.ino
