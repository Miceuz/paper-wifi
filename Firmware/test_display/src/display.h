#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "Arduino.h"
#include "sensor_readings.h"
#include "wifi_config.h"
// https://tchapi.github.io/Adafruit-GFX-Font-Customiser/

void DisplayData(SensorReadings sensor_readings, Settings settings,
                 bool is_wifi_active);
void DisplayInit();
void DisplayWifiInit(String ssid, uint32_t batt_voltage_mv,
                     bool is_wifi_active);
#endif