#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "env.h"

WiFiClientSecure espClient;
PubSubClient client(espClient);

char readingBuffer[5];

void setupWifi() {
  WiFi.disconnect(true);
  delay(500);

  Serial.print("Connecting to: ");
  Serial.println(SSID);

  WiFi.begin(SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.print(WiFi.localIP());
  Serial.print(" with MAC: ");
  Serial.println(WiFi.macAddress());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");

    if (client.connect(WiFi.macAddress().c_str(), MQTT_USER, MQTT_PASS)) {
      Serial.println("connected.");
    } else {
      Serial.print("failed, rc: ");
      Serial.print(client.state());
      Serial.println(" will retry in 5 seconds.");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  setupWifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);

  if (!client.connected()) {
    reconnect();
  }

  pinMode(D7, OUTPUT);
  digitalWrite(D7, LOW);
  int reading = 1024 - analogRead(A0);
  String readingString = String(reading);
  readingString.toCharArray(readingBuffer, readingString.length() + 1);
  client.publish("soil", readingBuffer);
  Serial.print("Sent reading: ");
  Serial.println(readingBuffer);

  ESP.deepSleep(1800 * 1000000); // Sleep every half hour
}

void loop() {
}
