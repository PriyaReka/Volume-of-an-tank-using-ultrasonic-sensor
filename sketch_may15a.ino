#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

#define TRIGGER_PIN 16
#define ECHO_PIN 17

const char* ssid = "Quantanics";
const char* password = "Qu@nt@nics18";
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char* clientId = "db032482-2f62-4ba8-9782-da6521020775";

unsigned long previousMillis = 0;
const long interval = 1000;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  client.setServer(mqttServer, mqttPort);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientId, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void publishData(float percentage){
  StaticJsonDocument<100> doc;
  doc["percentage"] = percentage;
  String jsonString;
  serializeJson(doc, jsonString);
  client.publish("/quantanics/industry/ultrasonic4", jsonString.c_str());
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    float v0 = 5687.97916, r1 = 7.5, h1 = 23.5;
    long duration;
    float h2, r2, k;
    
    digitalWrite(TRIGGER_PIN, LOW); 
    delayMicroseconds(2); 
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10); 
    digitalWrite(TRIGGER_PIN, LOW);
    
    duration = pulseIn(ECHO_PIN, HIGH);
    h2 = duration * 0.034 / 2;
    k = (v0 * 3) / (3.14 * h1);
    r2 = (-r1 + sqrt(-3 * r1 * r1 + 4 * k)) / 2; // Corrected the formula
    float v1 = (1.0 / 3.0) * 3.14 * (h1 - h2) * (r1 * r1 + r2 * r2 + r1 * r2); // Corrected the formula
    float percentage = (v1 / v0) * 100;
    Serial.println(percentage);
    publishData(percentage);
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

