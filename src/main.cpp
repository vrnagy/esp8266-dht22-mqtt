#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PubSubClient.h>
#include "config.h"

#define SECONDS_DS(seconds)  ((seconds)*1000000UL)

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (!client.connect(String(ESP.getChipId()).c_str())) {
      Serial.printf("failed, rc=%d try again in 1 second\n", client.state());
      delay(1000);  // Wait 1 second before retrying
    }
  }
}

void setup() {
  Serial.begin(74880);
  delay(10);
  dht.begin();
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
}

void loop() {
  delay(10);
  int i = 0;
  float h, t;
  bool hasErr;
  char msg[64], hum[10], temp[10];

  do {
    Serial.println("Measure");
    h = dht.readHumidity();
    t = dht.readTemperature();
    hasErr = (h == 0.0 || t == 0.0 || isnan(t) || isnan(h));
    if (i > 0) {
      delay(4000);
    }
    i++;
  } while (i < MAXRETRY && hasErr);

  if (!hasErr) {
    Serial.println("Temp: " + String(t) + "C Humidity: " + String(h) + "%");

    client.setServer(mqtt_server, mqtt_port);
    if (!client.connected()) {
      reconnect();
    }

    dtostrf(h, 4, 2, hum);
    dtostrf(t, 4, 2, temp);
    sprintf(msg, "{\"temperature\": %s, \"humidity\": %s }", temp, hum);

    client.publish(mqtt_topic.c_str(), msg, true);
  }

  Serial.println("Time to sleep");
  ESP.deepSleep(SECONDS_DS(300));
  delay(100);
}
