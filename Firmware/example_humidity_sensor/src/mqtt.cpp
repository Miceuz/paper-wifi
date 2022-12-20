#include "Arduino.h"
#include "PubSubClient.h"
#include "WiFiClient.h"
#include "sensor_readings.h"
#include "settings.h"
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
    if (strlen(settings.mqtt_username) && strlen(settings.mqtt_password)) {
      is_connected =
          mqtt_client.connect(settings.mqtt_device_id, settings.mqtt_username,
                              settings.mqtt_password);
    } else {
      is_connected = mqtt_client.connect(settings.mqtt_device_id);
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

void MqttSetup() {
  mqtt_client.setServer(settings.mqtt_address, settings.mqtt_port);
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

void MqttPublish(const SensorReadings &sensor_readings) {
  if (MqttConnect()) {
    char message[256];
    sensor_readings.asJSONString(message);
    Serial.println("Publishing message");
    mqtt_client.publish(settings.mqtt_topic, message);
    mqtt_client.disconnect();
  }
}