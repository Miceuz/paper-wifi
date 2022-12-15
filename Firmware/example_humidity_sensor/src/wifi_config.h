#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#include <stdbool.h>

#include "Arduino.h"
#include "WiFiManager.h"

struct Settings {
  enum class TempFormat : char { CELSIUS = 'C', FARENHEIT = 'F' };
  TempFormat temperature;
  bool send_mqtt;
  char mqtt_address[50];
  char mqtt_device_id[20];
  char mqtt_username[20];
  char mqtt_password[20];
  char mqtt_topic[20];
  uint16_t mqtt_port = 1883;
};

extern Settings settings;
extern WiFiManager wifi_manager;

void WifiConfigSetup();
#endif