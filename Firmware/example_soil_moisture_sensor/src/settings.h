#ifndef __SETTINGS_H
#define __SETTINGS_H

struct Settings {
  enum class TempFormat : char { CELSIUS = 'C', FARENHEIT = 'F' };
  enum class MoistFormat : char { RAW = 'r', PERCENT = '%' };
  TempFormat temp_format;
  MoistFormat moist_format;
  char mqtt_address[50];
  char mqtt_device_id[20];
  char mqtt_username[20];
  char mqtt_password[20];
  char mqtt_topic[20];
  uint16_t mqtt_port;
};

extern Settings settings;

#endif