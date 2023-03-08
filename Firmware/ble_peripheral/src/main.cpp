#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <SensirionI2CSht4x.h>

#define I2C_ENABLE 2
SensirionI2CSht4x sht4x;
float temperature = 0;
float humidity = 0;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define SERVICE_UUID_16 "4faf"
#define TEMPERATURE_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define HUMIDITY_UUID "c7d9f4d4-7c8d-4da4-b3db-2f22c8b793ba"

BLEServer *pServer = NULL;

BLECharacteristic *tempCharacteristic = NULL;
BLECharacteristic *humCharacteristic = NULL;

BLEAdvertising *pAdvertising;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param) {
    Serial.println("Connected");

    uint16_t intervalMin = 3 / 1.25e-3;
    uint16_t intervalMax = 4 / 1.25e-3;
    uint16_t timeout = 20 / 10e-3;
    uint16_t latency = 0;

    pServer->updateConnParams(param->connect.remote_bda, intervalMin,
                              intervalMax, latency, timeout);
  }

  void onDisconnect(BLEServer *pServer) {
    Serial.println("Disconnected");
    BLEDevice::startAdvertising();
  }
};

void setup() {
  pinMode(I2C_ENABLE, OUTPUT);
  digitalWrite(I2C_ENABLE, LOW);
  Wire.begin();
  sht4x.begin(Wire);

  Serial.begin(115200);
  BLEDevice::init("PaperWiFi");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  tempCharacteristic = pService->createCharacteristic(
      TEMPERATURE_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  tempCharacteristic->setValue(temperature);
  tempCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor *pDescriptor = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pDescriptor->setValue("Temperature (Celsius)");
  tempCharacteristic->addDescriptor(pDescriptor);

  humCharacteristic = pService->createCharacteristic(
      HUMIDITY_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  humCharacteristic->setValue(humidity);
  humCharacteristic->addDescriptor(new BLE2902());
  BLEDescriptor *pDescriptor2 = new BLEDescriptor(BLEUUID((uint16_t)0x2901));
  pDescriptor2->setValue("Humidity (%)");
  humCharacteristic->addDescriptor(pDescriptor2);

  pService->start();

  pAdvertising = pServer->getAdvertising();
  pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
  pAdvertising->setMinInterval(1 / 0.625e-3);
  pAdvertising->setMaxInterval(2 / 0.625e-3);
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
}

void AddFloat(String &s, float f) {
  char const *p = reinterpret_cast<char *>(&f);
  s += *p++;
  s += *p++;
  s += *p++;
  s += *p++;
}

void DecodeSensorData(String s, float &t, float &h) {
  char *sp = const_cast<char *>(s.c_str());
  sp += 2;
  float *f = reinterpret_cast<float *>(sp);
  t = *f;
  h = *(f+1);
}

void loop() {
  uint16_t error;
  char errorMessage[256];
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

    tempCharacteristic->setValue(temperature);
    tempCharacteristic->notify();

    humCharacteristic->setValue(humidity);
    humCharacteristic->notify();

    BLEAdvertisementData data;
    String s = "";
    // manufacturer code (0x02E5 for Espressif)
    s += (char)0xE5;
    s += (char)0x02;

    AddFloat(s, temperature);
    AddFloat(s, humidity);
    data.setName("PaperWifi");
    data.setFlags(ESP_BLE_ADV_FLAG_BREDR_NOT_SPT | ESP_BLE_ADV_FLAG_GEN_DISC);
    data.setCompleteServices(BLEUUID(SERVICE_UUID_16));
    data.setManufacturerData(s.c_str());

    pAdvertising->setAdvertisementData(data);
  }

  delay(3000);
}
