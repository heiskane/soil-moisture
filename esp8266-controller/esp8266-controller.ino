#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

const char* ssid = "Telia-987EE1";
const char* password = "SECRET_HERE";
const char* mqtt_server = "192.168.1.94";
const char* mqtt_user =   "potato";
const char* mqtt_pass =   "tomato";

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
const int S0 = 16;           // D0
const int S1 = 5;            // D1
const int S2 = 4;            // D2
const int mux_enable = 0;    // D4

const int mux_channels = 4;

int sensor_value = 0;

DynamicJsonDocument msg(512);

void reconnect() {
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqtt_client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
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

int read_mux(int channel) {
  digitalWrite(S0, bitRead(channel, 0));
  digitalWrite(S1, bitRead(channel, 1));
  digitalWrite(S2, bitRead(channel, 2));

  delay(100);
  return analogRead(analogInPin);  
}

void publish_data(char* sensor_name, int data) {
  if (!mqtt_client.connected()) {
    reconnect();
  }
  char tempString[512];

  msg["sensor"] = sensor_name;
  msg["value"] = data;
  serializeJson(msg, tempString);

  mqtt_client.publish("soil-moisture", tempString);
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
}

void loop() {
  delay(1000);
  Serial.println();
  
  for (int i = 0; i < mux_channels; i++) {    
    sensor_value = read_mux(i);

    // values under 120 probably mean the sensor is not connected
    if (sensor_value < 120) {
      continue;
    }

    char sensor_name[64];
    sprintf(sensor_name, "home-sensor-%d", i);
    Serial.println();

    char to_print[40];
    sprintf(to_print, "%s: %d", sensor_name, sensor_value);
    Serial.print(to_print);

    publish_data(sensor_name, sensor_value);
  }
}
