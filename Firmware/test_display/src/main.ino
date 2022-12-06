#include "WiFiManager.h"
#include "display.h"
#include "mqtt.h"
#include "sensor_readings.h"
#include "wifi_config.h"
#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

#define I2C_ENABLE 2
#define SECOND 1000000
#define MILLI_SECOND 1000

#define DEEP_SLEEP_TIME SECOND * 60 * 5
#define SENSOR_WARMUP_TIME MILLI_SECOND * 200

I2CSoilMoistureSensor sensor;
WiFiManager wifi_manager;

RTC_DATA_ATTR Settings settings;
RTC_DATA_ATTR SensorReadings sensor_readings;
RTC_DATA_ATTR bool is_wifi_active = false;
RTC_DATA_ATTR bool is_first_run = true;

String wifissidprefix = FPSTR("WIFI_PAPER_");
String ssid;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello");

  SensorsInit();
  SensorsPowerOn();
  SensorReadings new_readings = SensorsRead();
  SensorsPowerOff();

  DisplayInit();
  NetworkInit();

  if (is_first_run || IsSensorChangeSignificant(new_readings)) {
    DisplayData(new_readings, settings, is_wifi_active);
    if (is_wifi_active) {
      MqttPublish(new_readings);
    }
    sensor_readings = new_readings;
  }
  is_first_run = false;
  DeepSleep(DEEP_SLEEP_TIME);
}

bool IsSensorChangeSignificant(SensorReadings new_readings) {
  return settings.moisture == Settings::MoistFormat::RAW
             ? sensor_readings != new_readings
             : sensor_readings.moistureAsPercent() !=
                   new_readings.moistureAsPercent();
}

void NetworkInit() {
  ssid = wifissidprefix + String(WIFI_getChipId(), HEX);

  if (is_first_run) {
    Serial.println(ssid);
    DisplayWifiInit(ssid, sensor_readings.batt_voltage_mv, true);
    wifi_manager.resetSettings();
    settings.mqtt_device_id = ssid;
    WifiConfigSetup();
  }

  if (is_first_run || is_wifi_active) {
    is_wifi_active = wifi_manager.autoConnect(ssid.c_str());
  }

  if (is_wifi_active) {
    settings.mqtt_address = "test.mosquitto.org";
    MqttSetup(settings);
  }
}

void SensorsInit() {
  pinMode(I2C_ENABLE, OUTPUT);
  digitalWrite(I2C_ENABLE, LOW);

  Wire.begin();
  sensor_readings.batt_voltage_mv = analogReadMilliVolts(A3) * 2;
}

void SensorsPowerOn() {
  digitalWrite(I2C_ENABLE, LOW);
  esp_sleep_enable_timer_wakeup(SENSOR_WARMUP_TIME);
  esp_light_sleep_start();
  // delay(100);
}

SensorReadings SensorsRead() {
  SensorReadings sensor_readings;
  sensor_readings.batt_voltage_mv = analogReadMilliVolts(A3) * 2;

  uint8_t averages_m = 0, averages_t = 0;
  uint16_t m = 0, t = 0, d = 0;

  for (uint8_t i = 0; i < 10; i++) {
    d = sensor.getCapacitance();
    if (d != 65535) {
      m += d;
      averages_m++;
    }
    d = sensor.getTemperature();
    if (d != 65535) {
      t += d;
      averages_t++;
    }
  }
  if (averages_m) {
    sensor_readings.moisture = m / averages_m;
  }

  if (averages_t) {
    sensor_readings.temperature = t / averages_t;
  }
  Serial.println(String("Moisture: ") + sensor_readings.moisture);
  Serial.println(String("Temperature: ") + sensor_readings.temperature);
  Serial.println(String("Moisture percent: ") +
                 sensor_readings.moistureAsPercent());

  return sensor_readings;
}

void SensorsPowerOff() { digitalWrite(I2C_ENABLE, HIGH); }

void DeepSleep(uint32_t us) {
  esp_sleep_enable_timer_wakeup(us);
  esp_deep_sleep_start();
}

void loop() {}
