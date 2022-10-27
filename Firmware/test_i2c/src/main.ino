#include <I2CSoilMoistureSensor.h>
#include <Wire.h>
// #include <driver/temp_sensor.h>

#include "SPI.h"

I2CSoilMoistureSensor sensor;

#define I2C_ENABLE 2

RTC_DATA_ATTR int bootCount = 0;

void setup() {
  bootCount++;
  pinMode(I2C_ENABLE, OUTPUT);
  digitalWrite(I2C_ENABLE, LOW);
  Serial.begin(9600);

  delay(1000);
  Serial.println("I2C functionality test");
  Wire.begin(4, 5, 19000);
  // sensor.begin(); // reset sensor
  delay(1000); // give some time to boot up
  Serial.print("I2C Soil Moisture Sensor Address: ");
  Serial.println(sensor.getAddress(), HEX);
  Serial.print("Sensor Firmware version: ");
  Serial.println(sensor.getVersion(), HEX);
}

void loop() {
  // while (sensor.isBusy())
  //   delay(50); // available since FW 2.3
  // Serial.print("Soil Moisture Capacitance: ");
  Serial.print(sensor.getCapacitance()); // read capacitance register
  // Serial.print(", Temperature: ");
  // Serial.print(sensor.getTemperature() / (float)10); // temperature register
  // Serial.print(" Battery: ");
  // Serial.print(analogReadMilliVolts(A3) * 2 / 1000.0);
  // Serial.print(" Temperature: ");
  // Serial.print(temperatureRead());

  // Serial.print(", Light: ");
  // Serial.println(sensor.getLight(
  //     true)); // request light measurement, wait and read light register
  // sensor.sleep(); // available since FW 2.3
  Serial.println();
  // esp_sleep_enable_timer_wakeup(1000000);
  // esp_deep_sleep_start();
}