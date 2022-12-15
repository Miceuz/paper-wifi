#include <GxEPD.h>
#include <GxGDEW0213T5D/GxGDEW0213T5D.h> // 2.13" b/w 104x212 UC8151D
#include <GxIO/GxIO.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>

#include "FreeMonoBold36.h"
#include "bitmaps.h"
#include "qrcode.h"
#include "symbols.h"
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>

#include "Arduino.h"
#include "display.h"
#include <stdint.h>

#define CS 20
#define DC 9
#define RST 8
#define BUSY 10

GxIO_Class io(SPI, /*CS=*/20, /*DC=*/9, /*RST=*/8);
GxEPD_Class display(io, /*RST=*/8, /*BUSY=*/10);

const int backgroundColor = GxEPD_WHITE;
const int foregroundColor = GxEPD_BLACK;

void DisplayInit() {
  display.init(115200);
  display.setRotation(3);
  display.setTextColor(foregroundColor);
}

static void DisplayQrCode(uint8_t at_x, uint8_t at_y, String message) {
  QRCode qrcode;
  // See table at https://github.com/ricmoo/QRCode
  // or https://www.qrcode.com/en/about/version.html for
  // calculation of data capacity of a QR code. Current
  // settings will support a string of about 100 bytes:
  int qrVersion = 4;
  // can be ECC_LOW, ECC_MEDIUM, ECC_QUARTILE and ECC_HIGH (0-3, respectively):
  int qrErrorLevel = ECC_LOW;

  // allocate QR code memory:
  byte qrcodeBytes[qrcode_getBufferSize(qrVersion)];
  qrcode_initText(&qrcode, qrcodeBytes, qrVersion, qrErrorLevel,
                  message.c_str());

  // QR Code block characteristics will depend on the display:
  // QR code needs a "quiet zone" of background color around it, hence the
  // offset:
  // int offset = 10;
  int blockSize = 1;
  //(display.height() - (offset * 2)) / qrcode.size;

  for (byte y = 0; y < qrcode.size; y++) {
    for (byte x = 0; x < qrcode.size; x++) {
      int blockX = (x * blockSize) + at_x;
      int blockY = (y * blockSize) + at_y;
      int blockValue = qrcode_getModule(&qrcode, x, y);
      int blockColor = backgroundColor;
      if (blockValue == 1) {
        blockColor = foregroundColor;
      }
      display.fillRect(blockX, blockY, blockSize, blockSize, blockColor);
    }
  }
}

void DrawBatteryAndWifiLevel(uint32_t batt_voltage_mv, bool is_wifi_active);

void DisplayWifiInit(const String &ssid, uint32_t batt_voltage_mv,
                     bool is_wifi_active) {
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.fillScreen(backgroundColor);

  DrawBatteryAndWifiLevel(batt_voltage_mv, is_wifi_active);

  display.drawBitmap(6, (104 - 53) / 2 - 20, epd_bitmap_catnip, 200, 53,
                     foregroundColor);
  char msg[50];
  sprintf(msg, "WIFI:T:nopass;S:%s;P:;H:;;", ssid.c_str());
  DisplayQrCode(10, 65, msg);
  display.setFont(&TomThumb);
  display.setCursor(50, 70);
  display.print("Scan the barcode to connec to WiFi");
  display.setCursor(50, 80);
  sprintf(msg, "WiFi SSID: %s", ssid.c_str());
  display.print(msg);
  display.setCursor(50, 90);
  display.print("WiFi setup will be skipped in 120 seconds.");
  display.update();
}

char msg[25];

void DisplayData(const SensorReadings &sensor_readings,
                 const Settings &settings, const bool is_wifi_active) {
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.setRotation(3);

  display.fillScreen(backgroundColor);
  display.setTextColor(foregroundColor);

  sprintf(msg, "%d%s",
          settings.moisture == Settings::MoistFormat::PERCENT
              ? sensor_readings.moistureAsPercent()
              : sensor_readings.moisture,
          settings.moisture == Settings::MoistFormat::PERCENT ? "%" : "");

  display.setFont(&FreeMono_Bold36pt7b);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setCursor(x, y);
  display.print(msg);

  DrawBatteryAndWifiLevel(sensor_readings.batt_voltage_mv, is_wifi_active);
  // sprintf(msg, "%.1fV", (float)batt_voltage_mv / 1000.0);
  // display.setFont(&FreeMonoBold9pt7b);
  // display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  // display.setCursor(1, tbh + 1);
  // display.print(msg);

  sprintf(msg, "%.1f%s",
          settings.temperature == Settings::TempFormat::CELSIUS
              ? sensor_readings.temperature
              : sensor_readings.temperatureAsFarenheit(),
          settings.temperature == Settings::TempFormat::CELSIUS ? "C" : "F");
  display.setFont(&FreeMonoBold9pt7b);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(1, tbh + 1);
  display.print(msg);

  display.update();
}

char ToBatteryLevelSymbol(uint32_t batt_voltage_mv) {
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

void DrawBatteryAndWifiLevel(uint32_t batt_voltage_mv, bool is_wifi_active) {
  Serial.println(String("BATT VOLTAGE in DISPLAY:") + batt_voltage_mv);
  Serial.println(String("SYMBOL:") +ToBatteryLevelSymbol(batt_voltage_mv));
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  sprintf(msg, "%c %c", is_wifi_active ? 'W' : ' ',
          ToBatteryLevelSymbol(batt_voltage_mv));
  display.setFont(&symbols);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor(display.width() - tbw - 2, tbh + 1);
  display.print(msg);
}

void drawBatteryEmpty() {
  int16_t tbx, tby;
  uint16_t tbw, tbh;

  display.fillScreen(backgroundColor);
  sprintf(msg, "E");
  display.setFont(&symbols);
  display.getTextBounds(msg, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setCursor(x, y);
  display.print(msg);
  display.update();
}