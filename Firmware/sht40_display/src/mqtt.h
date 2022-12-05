#ifndef _MQTT_H
#define _MQTT_H
#include "PubSubClient.h"
#include "sensor_readings.h"
#include "wifi_config.h"

void MqttSetup(Settings settings);
void MqttPublish(SensorReadings sensor_readings);
#endif