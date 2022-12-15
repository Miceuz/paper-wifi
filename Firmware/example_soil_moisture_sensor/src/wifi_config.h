#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#include <stdbool.h>

#include "Arduino.h"
#include "WiFiManager.h"

struct Settings {
  enum class TempFormat : char { CELSIUS = 'C', FARENHEIT = 'F' };
  enum class MoistFormat : char { RAW = 'r', PERCENT = '%' };
  TempFormat temperature;
  MoistFormat moisture;
  bool send_mqtt;
  char mqtt_address[50];
  char mqtt_device_id[20];
  char mqtt_username[20];
  char mqtt_password[20];
  char mqtt_topic[20];
  uint16_t mqtt_port;
};

extern Settings settings;
extern WiFiManager wifi_manager;

void WifiConfigSetup();
#endif