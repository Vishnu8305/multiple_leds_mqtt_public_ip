#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "xxxxxxxx";//wifi ssid 
const char* password = "xxxxxxxxxx"; // wifi password

// MQTT broker details
const char* mqtt_broker = "xxxxxxxxx";//( broker.hivemq.com ) mqtt broker 
const int mqtt_port = 1883;//mqqtt port
const char* commandTopics[] = {
    "home/switch1", "home/switch2", "home/switch3", "home/switch4",
    "home/switch5", "home/switch6", "home/switch7", "home/switch8",
    "home/switch9", "home/switch10", "home/switch11", "home/switch12"
};               // mqtt topic
const int ledPins[] = {15, 2, 0, 4, 16, 17, 5, 18, 19, 13, 12, 14};

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Initialize LED pins
  for (int i = 0; i < 12; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Connect to MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32Receiver")) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, state: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Subscribe to all command topics
  for (int i = 0; i < 12; i++) {
    client.subscribe(commandTopics[i]);
    Serial.print("Subscribed to topic: ");
    Serial.println(commandTopics[i]);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  // Handle commands
  for (int i = 0; i < 12; i++) {
    if (strcmp(topic, commandTopics[i]) == 0) {
      if (message == "true") {
        digitalWrite(ledPins[i], HIGH);
        Serial.print("LED ");
        Serial.print(i + 1);
        Serial.println(" ON");
      } else if (message == "false") {
        digitalWrite(ledPins[i], LOW);
        Serial.print("LED ");
        Serial.print(i + 1);
        Serial.println(" OFF");
      } else {
        Serial.println("Unknown command received");
      }
      break;
    }
  }
}

void loop() {
  client.loop();
}
