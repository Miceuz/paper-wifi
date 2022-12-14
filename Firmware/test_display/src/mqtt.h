#ifndef _MQTT_H
#define _MQTT_H
#include "PubSubClient.h"
#include "sensor_readings.h"
#include "wifi_config.h"

void MqttSetup();
void MqttPublish(const SensorReadings &sensor_readings);
#endif