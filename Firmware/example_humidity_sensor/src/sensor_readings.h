#ifndef __SENSOR_READINGS_H
#define __SENSOR_READINGS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "settings.h"

struct SensorReadings {
  using TempFormat = Settings::TempFormat;
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
            "{\"humidity\": \"%.2f\", \"temperature\": \"%.2f\", "
            "\"battery_voltage_mv\":\"%d\"}",
            humidity, temperature, batt_voltage_mv);
  }
  float temperatureAs(const TempFormat format) const {
    return format == TempFormat::FARENHEIT ? toFarenheit(temperature)
                                           : temperature;
  }

  char temperatureUnits(const TempFormat format) const {
    return format == TempFormat::FARENHEIT ? 'F' : 'C';
  }

  char batteryVoltageAsSymbol() const {
    if (batt_voltage_mv > 2750) {
      return 'A';
    } else if (batt_voltage_mv > 2500) {
      return 'B';
    } else if (batt_voltage_mv > 2250) {
      return 'C';
    } else {
      return 'D';
    }
  }

 private:
  float toFarenheit(float t) const { return t * 9 / 5 + 32; }
};

#endif