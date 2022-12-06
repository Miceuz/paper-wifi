#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#include "Arduino.h"
#include "WiFiManager.h"
#include <stdbool.h>

struct Settings {
  enum class TempFormat : char { CELSIUS = 'C', FARENHEIT = 'F' };
  enum class MoistFormat : char { RAW = 'r', PERCENT = '%' };
  TempFormat temperature = TempFormat::CELSIUS;
  MoistFormat moisture = MoistFormat::PERCENT;
  bool send_mqtt = false;
  String mqtt_address;
  String mqtt_device_id;
  String mqtt_username;
  String mqtt_password;
};

extern WiFiManagerParameter temp_format_param;
extern WiFiManagerParameter moist_format_param;
extern WiFiManagerParameter mqtt_checkbox_param;
extern WiFiManagerParameter mqtt_broker_address_param;

extern Settings settings;
extern WiFiManager wifi_manager;

void WifiConfigSetup();
#endif