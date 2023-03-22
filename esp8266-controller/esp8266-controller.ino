#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

const char* ssid = "Telia-987EE1";
const char* password = "SECRET_HERE";
const char* mqtt_server = "192.168.1.94";

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
const int S0 = 16;           // D0
const int S1 = 5;            // D1
const int S2 = 4;            // D2
const int mux_enable = 0;    // D4

int sensor_value = 0;
int mux_channels = 4;

DynamicJsonDocument msg(512);

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

  pinMode(mux_enable, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(analogInPin, INPUT);

  digitalWrite(mux_enable, LOW);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
}

void loop() {

  Serial.println();
  
  for (int i = 0; i < mux_channels; i++) {    
    digitalWrite(S0, bitRead(i, 0));
    digitalWrite(S1, bitRead(i, 1));
    digitalWrite(S2, bitRead(i, 2));

    sensor_value = analogRead(analogInPin);
    Serial.print("sensor");
    Serial.print(i + 1);
    Serial.print("_value: ");
    Serial.println(sensor_value);

    if (!mqtt_client.connected()) {
      reconnect();
    }

    char sensor_name[64];
    char tempString[512];

    sprintf(sensor_name, "home-sensor-%d", i);

    msg["sensor"] = sensor_name;
    msg["value"] = sensor_value;
    serializeJson(msg, tempString);

    mqtt_client.publish("soil-moisture", tempString);
      delay(100);
    }


  delay(1000);
}
