#include <SensirionI2CSht4x.h>

#define I2C_ENABLE 2
SensirionI2CSht4x sht4x;

void printUint16Hex(uint16_t value) {
  Serial.print(value < 4096 ? "0" : "");
  Serial.print(value < 256 ? "0" : "");
  Serial.print(value < 16 ? "0" : "");
  Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
  Serial.print("Serial: 0x");
  printUint16Hex(serial0);
  printUint16Hex(serial1);
  printUint16Hex(serial2);
  Serial.println();
}

void setup() {
  pinMode(I2C_ENABLE, OUTPUT);
  digitalWrite(I2C_ENABLE, LOW);

  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }
  delay(5000);
  Wire.begin();
  uint16_t error;
  char errorMessage[256];

  sht4x.begin(Wire);

  uint32_t serialNumber;
  error = sht4x.serialNumber(serialNumber);
  if (error) {
    Serial.print("Error trying to execute serialNumber(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.print("Serial Number: ");
    Serial.println(serialNumber);
  }
}
void loop() {
  uint16_t error;
  char errorMessage[256];

  delay(1000);

  float temperature;
  float humidity;
  error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    Serial.print("Error trying to execute measureHighPrecision(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    Serial.print("Temperature:");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print("Humidity:");
    Serial.println(humidity);
  }
}