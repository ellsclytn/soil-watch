#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "ThingSpeak.h"
#include "env.h"

WiFiClient espClient;
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

    if (client.connect(WiFi.macAddress().c_str(), USER, MQ_PASS)) {
      Serial.println("connected.");
      client.subscribe("pr");
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
  client.setServer(SERVER, PORT);
  ThingSpeak.begin(espClient);

  if (!client.connected()) {
    reconnect();
  }

  int reading = analogRead(A0);
  String readingString = String(reading);
  readingString.toCharArray(readingBuffer, readingString.length() + 1);
  client.publish("soilMoisture", readingBuffer);
  ThingSpeak.writeField(TS_CHANNEL_ID, 2, readingBuffer, TS_API_KEY);
  Serial.print("Sent reading: ");
  Serial.println(readingBuffer);

  ESP.deepSleep(1800 * 1000000); // Sleep every half hour
}

void loop() {
}

