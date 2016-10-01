#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <PubSubClient.h>
#include "ThingSpeak.h"
#include "env.h"

Adafruit_ADS1115 ads;
WiFiClient espClient;
PubSubClient client(espClient);

char readingBuffer[7];

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

  ads.begin();
  setupWifi();
  client.setServer(SERVER, PORT);
  ThingSpeak.begin(espClient);

  if (!client.connected()) {
    reconnect();
  }

  int16_t reading = ads.readADC_SingleEnded(0);
  String readingString = String(reading);
  readingString.toCharArray(readingBuffer, readingString.length() + 1);
  client.publish("soilMoisture", readingBuffer);
  ThingSpeak.writeField(TS_CHANNEL_ID, 1, readingBuffer, TS_API_KEY);
  Serial.print("Sent reading: ");
  Serial.println(readingBuffer);

  ESP.deepSleep(900 * 1000000); // Sleep every 15 minutes
}

void loop() {
}

