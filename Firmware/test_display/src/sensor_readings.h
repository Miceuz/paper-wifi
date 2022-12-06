#ifndef __SENSOR_READINGS_H
#define __SENSOR_READINGS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct SensorReadings {
  const uint32_t moisture_low = 250;
  const uint32_t moisture_high = 532;

  uint32_t temperature;
  uint32_t moisture;
  uint32_t batt_voltage_mv;

  SensorReadings &operator=(const SensorReadings &a) {
    temperature = a.temperature;
    moisture = a.moisture;
    batt_voltage_mv = a.batt_voltage_mv;
    return *this;
  }

  bool operator==(const SensorReadings &a) {
    return temperature == a.temperature && moisture == a.moisture &&
           batt_voltage_mv == a.batt_voltage_mv;
  }

  bool operator!=(const SensorReadings &a) {
    return abs((int32_t)temperature - (int32_t)a.temperature) > 10 ||
           abs((int32_t)moisture - (int32_t)a.moisture) > 1 ||
           abs((int32_t)batt_voltage_mv - (int32_t)a.batt_voltage_mv) > 100;
  }

  void asJSONString(char *message) {
    sprintf(message,
            "{\"moisture\": \"%f\", \"temperature\": \"%f\", "
            "\"battery_voltage_mv\":\"%f\"}",
            moisture, temperature, batt_voltage_mv);
  }

  uint32_t moistureAsPercent() {
    if (moisture < moisture_low) {
      return 0;
    }
    if (moisture > moisture_high) {
      return 100;
    }
    return (moisture - moisture_low) * 100 / (moisture_high - moisture_low);
  }
};

#endif