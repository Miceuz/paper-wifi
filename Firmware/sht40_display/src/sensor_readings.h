#ifndef __SENSOR_READINGS_H
#define __SENSOR_READINGS_H

#include <stdint.h>
#include <stdio.h>

struct SensorReadings {
  float temperature;
  float humidity;
  uint32_t batt_voltage_mv;

  SensorReadings &operator=(const SensorReadings &a) {
    temperature = a.temperature;
    humidity = a.humidity;
    batt_voltage_mv = a.batt_voltage_mv;
    return *this;
  }

  bool operator==(const SensorReadings &a) {
    return temperature == a.temperature && humidity == a.humidity &&
           batt_voltage_mv == a.batt_voltage_mv;
  }

  bool operator!=(const SensorReadings &a) {
    return temperature != a.temperature || humidity != a.humidity ||
           batt_voltage_mv != a.batt_voltage_mv;
  }

  void asJSONString(char *message) const {
    sprintf(message,
            "{\"humidity\": \"%f\", \"temperature\": \"%f\", "
            "\"battery_voltage_mv\":\"%d\"}",
            humidity, temperature, batt_voltage_mv);
  }

  float temperatureAsFarenheit() const { return temperature * 9 / 5 + 32; }
};

#endif