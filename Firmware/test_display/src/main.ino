#include <GxEPD.h>
#include <GxGDEW0213T5D/GxGDEW0213T5D.h> // 2.13" b/w 104x212 UC8151D
#include <GxIO/GxIO.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>

#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#include <I2CSoilMoistureSensor.h>

I2CSoilMoistureSensor sensor;
#define I2C_ENABLE 2

#define CS 20
#define DC 9
#define RST 8
#define BUSY 10

GxIO_Class io(SPI, /*CS=*/20, /*DC=*/9,
              /*RST=*/8); // arbitrary selection of 8, 9 selected for default of
                          // GxEPD_Class
GxEPD_Class display(io, /*RST=*/8, /*BUSY=*/10); // default selection of (9), 7

void setup() {
  pinMode(I2C_ENABLE, OUTPUT);
  digitalWrite(I2C_ENABLE, LOW);

  Wire.begin();
  Wire.setClock(19000);
  display.init(115200);
}

char msg[25];

RTC_DATA_ATTR uint16_t moisture = 0;
RTC_DATA_ATTR uint16_t temperature = 0;
RTC_DATA_ATTR uint32_t batt_voltage_mv = 0;

void drawMsg() {
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.setRotation(1);

  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);

  sprintf(msg, "%d", moisture);
  display.setFont(&FreeMonoBold24pt7b);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setCursor(x, y);
  display.print(msg);

  sprintf(msg, "%.1fV", (float)batt_voltage_mv / 1000.0);
  display.setFont(&FreeMonoBold9pt7b);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(1, tbh + 1);
  display.print(msg);

  sprintf(msg, "%.1fC", (float)temperature / 10.0);
  display.setFont(&FreeMonoBold9pt7b);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(display.width() - tbw - 1, tbh + 1);
  display.print(msg);

  display.update();
}

void loop() {
  digitalWrite(I2C_ENABLE, LOW);
  esp_sleep_enable_timer_wakeup(200000);
  esp_light_sleep_start();
  // delay(100);
  uint16_t m = sensor.getCapacitance();
  uint16_t t = sensor.getTemperature();
  digitalWrite(I2C_ENABLE, HIGH);

  uint32_t b = analogReadMilliVolts(A3) * 2;

  if (moisture != m) {
    moisture = m;
    temperature = t;
    batt_voltage_mv = b;
    drawMsg();
    Serial.println(m);
  }

  esp_sleep_enable_timer_wakeup(60000000);
  esp_deep_sleep_start();
}