#ifndef __SENSOR_READINGS_H
#define __SENSOR_READINGS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "settings.h"

struct SensorReadings {
  using MoistFormat = Settings::MoistFormat;
  using TempFormat = Settings::TempFormat;

  const uint32_t moisture_low = 250;
  const uint32_t moisture_high = 532;

  float temperature;
  uint32_t moisture;
  uint32_t batt_voltage_mv;

  SensorReadings &operator=(const SensorReadings &a) {
    temperature = a.temperature;
    moisture = a.moisture;
    batt_voltage_mv = a.batt_voltage_mv;
    return *this;
  }

  bool operator==(const SensorReadings &a) {
    return temperature == a.temperature &&
           moistureAs(settings.moist_format) ==
               a.moistureAs(settings.moist_format) &&
           batt_voltage_mv == a.batt_voltage_mv;
  }

  bool operator!=(const SensorReadings &a) {
    return abs(temperature - a.temperature) > 0.1 ||
           abs((int32_t)moistureAs(settings.moist_format) -
               (int32_t)a.moistureAs(settings.moist_format)) > 1 ||
           abs((int32_t)batt_voltage_mv - (int32_t)a.batt_voltage_mv) > 100;
  }

  void asJSONString(char *message) const {
    sprintf(message,
            "{\"moisture\": \"%d\", \"temperature\": \"%.1f\", "
            "\"battery_voltage_mv\":\"%d\"}",
            moisture, temperature, batt_voltage_mv);
  }

  uint32_t moistureAs(const MoistFormat format) const {
    return format == MoistFormat::PERCENT ? toPercent(moisture) : moisture;
  }

  char moistureUnits(const MoistFormat format) const {
    return format == MoistFormat::PERCENT ? '%' : '\0';
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

  uint32_t toPercent(uint32_t m) const {
    if (m < moisture_low) {
      return 0;
    }
    if (m > moisture_high) {
      return 100;
    }
    return (m - moisture_low) * 100 / (moisture_high - moisture_low);
  }
};

#endif