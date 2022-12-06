#include "Arduino.h"
#include "PubSubClient.h"
#include "WiFiClient.h"
#include "sensor_readings.h"
#include "wifi_config.h"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void mqtt_callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

bool mqtt_reconnect() {
  uint32_t retries = 0;
  // Loop until we're reconnected
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    bool is_connected = false;
    if (settings.mqtt_username && settings.mqtt_password) {
      is_connected = mqtt_client.connect(settings.mqtt_device_id.c_str(),
                                         settings.mqtt_username.c_str(),
                                         settings.mqtt_password.c_str());
    } else {
      is_connected = mqtt_client.connect(settings.mqtt_device_id.c_str());
    }

    if (!is_connected) {
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      if (retries++ > 5) {
        Serial.println("Giving up");
        return false;
      }
      Serial.println(" try again in 1 second");
      delay(1000);
    }
  }
  return true;
}

// PubSubClient is not copying mqtt server address, rather just performs pointer
// assignment, thus we create a C string in memory so that PubSubClient could
// have it
char mqtt_address[50];
void MqttSetup(Settings settings) {
  strcpy(mqtt_address, settings.mqtt_address.c_str());
  mqtt_client.setServer(mqtt_address, 1883);
  mqtt_client.setCallback(mqtt_callback);
}

bool MqttConnect() {
  if (!mqtt_client.connected()) {
    if (!mqtt_reconnect()) {
      return false;
    }
  }
  mqtt_client.loop();
  return true;
}

void MqttPublish(SensorReadings sensor_readings) {
  if (MqttConnect()) {
    char message[256];
    sensor_readings.asJSONString(message);
    Serial.println("Publishing message");
    mqtt_client.publish("lt.catnip.test", message);
    mqtt_client.disconnect();
  }
}