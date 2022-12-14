#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#include "Arduino.h"
#include "WiFiManager.h"
#include <stdbool.h>

struct Settings {
  enum class TempFormat : char { CELSIUS = 'C', FARENHEIT = 'F' };
  TempFormat temperature = TempFormat::CELSIUS;
  bool send_mqtt = false;
  String mqtt_address;
  String mqtt_device_id;
  String mqtt_username;
  String mqtt_password;
  String mqtt_topic;
  uint16_t mqtt_port = 1883;
};

extern Settings settings;
extern WiFiManager wifi_manager;

void WifiConfigSetup();
#endif