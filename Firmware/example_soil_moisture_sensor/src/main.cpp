#include <I2CSoilMoistureSensor.h>
#include <Wire.h>

#include "WiFiManager.h"
#include "display.h"
#include "mqtt.h"
#include "sensor_readings.h"
#include "wifi_config.h"
#include "average.h"

#define I2C_ENABLE 2
#define WAKEUP_PIN 0
#define IO1 1
#define SECOND 1000000
#define MILLI_SECOND 1000

#define DEEP_SLEEP_TIME SECOND * 60 * 5
#define SENSOR_WARMUP_TIME MILLI_SECOND * 200

I2CSoilMoistureSensor sensor;
WiFiManager wifi_manager;

RTC_DATA_ATTR Settings settings;
RTC_DATA_ATTR SensorReadings sensor_readings;
RTC_DATA_ATTR bool is_wifi_configured = false;
RTC_DATA_ATTR bool is_wifi_connected = false;
RTC_DATA_ATTR uint8_t connect_retries = 0;
RTC_DATA_ATTR bool is_first_run = true;

String wifissidprefix = FPSTR("PAPER_WIFI_");
String ssid;

void SensorsInit();
void SensorsPowerOn();
SensorReadings SensorsRead();
void SensorsPowerOff();
void NetworkInit();
void DeepSleep(uint32_t us);
void ConfigureButton();

void setup() {
  Serial.begin(9600);
  ConfigureButton();
  if (is_first_run) {
    Serial.println(String("Hello, this is the first run after reset."));
  } else {
    Serial.println("Wakeup!");
  }

  SensorsInit();
  SensorsPowerOn();
  SensorReadings new_readings = SensorsRead();
  if (is_first_run) {
    sensor_readings = new_readings;
  }
  SensorsPowerOff();

  if (is_first_run || sensor_readings != new_readings) {
    DisplayInit();
    NetworkInit();
    DisplayData(new_readings, is_wifi_connected);
    if (is_wifi_connected) {
      Serial.println("Wifi is connected");
      MqttSetup();
      MqttPublish(new_readings);
    } else {
      Serial.println("Wifi is not connected");
      if (is_wifi_configured) {
        Serial.println("Wifi is configured.");
      } else {
        Serial.println("Wifi is not configured.");
      }
    }
    sensor_readings = new_readings;
  }
  is_first_run = false;
  DeepSleep(DEEP_SLEEP_TIME);
}

void NetworkInit() {
  ssid = wifissidprefix + String((uint32_t)(ESP.getEfuseMac() >> 16), HEX);
  if (is_first_run) {
    WiFi.mode(WIFI_STA);
    WiFi.mode(WIFI_AP);
    Serial.println(ssid);
    DisplayWifiInit(ssid, sensor_readings, true);
    wifi_manager.resetSettings();
    ssid.toCharArray(settings.mqtt_device_id, 20, 0);
    WifiConfigSetup();
  }

  if (is_wifi_configured) {
    wifi_manager.setEnableConfigPortal(false);
  }

  if (is_first_run || is_wifi_configured) {
    is_wifi_connected = wifi_manager.autoConnect(ssid.c_str());

    if (is_wifi_connected) {
      connect_retries = 0;
      is_wifi_configured = true;
    } else {
      if (is_first_run && !is_params_saved) {
        settings.setDefault();
      }
      // if we can't connect to WiFi after it has been successfully configured,
      // we give up eventually. Say, device was configured to WiFi hostspot and
      // then brought too far from the router. More logic can be added here,
      // like a try to recover a known connection or display the connection
      // screen again, etc
      if (connect_retries++ > 3) {
        is_wifi_configured = false;
      }
    }
  }
  Serial.println("Network config done");
}

void SensorsInit() {
  pinMode(I2C_ENABLE, OUTPUT);
  digitalWrite(I2C_ENABLE, LOW);

  Wire.begin();
  Wire.setClock(10000);
  sensor_readings.batt_voltage_mv = analogReadMilliVolts(A3) * 2;
}

void SensorsPowerOn() {
  digitalWrite(I2C_ENABLE, LOW);
  esp_sleep_enable_timer_wakeup(SENSOR_WARMUP_TIME);
  esp_light_sleep_start();
  // delay(100);
}

SensorReadings SensorsRead() {
  auto get_capacitance = [] { return sensor.getCapacitance(); };
  auto get_temperature = [] { return sensor.getTemperature(); };
  SensorReadings sensor_readings;
  sensor_readings.batt_voltage_mv = analogReadMilliVolts(A3) * 2;
  sensor_readings.moisture = Average<unsigned int>(get_capacitance);
  sensor_readings.temperature = static_cast<float>(Average<int>(get_temperature)) / 10;

  Serial.println(String("Temperature: ") + sensor_readings.temperature);
  Serial.println(String("Moisture raw: ") + sensor_readings.moisture);
  Serial.println(String("Moisture percent: ") +
                 sensor_readings.moistureAs(Settings::MoistFormat::PERCENT));
  Serial.println(String("Battery mV: ") + sensor_readings.batt_voltage_mv);
  return sensor_readings;
}

void SensorsPowerOff() { digitalWrite(I2C_ENABLE, HIGH); }

void DeepSleep(uint32_t us) {
  esp_deep_sleep_enable_gpio_wakeup(1 << WAKEUP_PIN,
                                    ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_sleep_enable_timer_wakeup(us);
  esp_deep_sleep_start();
}

void ButtonInterruptHandler() {
  // nothing, just wake up
}

void ConfigureButton() {
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
  attachInterrupt(WAKEUP_PIN, ButtonInterruptHandler, FALLING);
}

void loop() {}
