#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "Arduino.h"
#include "sensor_readings.h"
#include "wifi_config.h"

// https://tchapi.github.io/Adafruit-GFX-Font-Customiser/

void DisplayInit();
void DisplayData(const SensorReadings &sensor_readings,
                 const bool is_wifi_active);
void DisplayWifiInit(const String &ssid, const SensorReadings &sensor_readings,
                     const bool is_wifi_active);
#endif