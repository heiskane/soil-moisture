#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

const char* ssid = "Telia-987EE1";
const char* password = "SECRET_HERE";
const char* mqtt_server = "192.168.1.94";

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
int sensor_value = 0;

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  mqtt_client.setServer(mqtt_server, 1883);
  reconnect();
}

void loop() {
  sensor_value = analogRead(analogInPin);
  Serial.print("sensor_value: ");
  Serial.println(sensor_value);

  char tempString[8];
  dtostrf(sensor_value, 1, 0, tempString);

  if (!mqtt_client.connected()) {
    reconnect();
  }

  mqtt_client.publish("soil-moisture", tempString);
  delay(1000);
}
