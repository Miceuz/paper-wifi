#include "wifi_config.h"

#include "WiFiManager.h"
#include "settings.h"

const char *temp_radio_str =
    "<br/><label for='temp_format'>Temperature format:<br/></label>"
    "<input type = 'radio' name = 'temp_format' value = 'C' checked /> "
    "Celsius<br/>"
    "<input type = 'radio' name = 'temp_format' value = 'F'> Farenheit ";
WiFiManagerParameter temp_format_param(temp_radio_str);

const char *primary_reading_radio_str =
    "<br/><label for='temp_format'>Primary data display:<br/></label>"
    "<input type = 'radio' name = 'primary_reading' value = 'M' checked /> "
    "Soil moisture<br/>"
    "<input type = 'radio' name = 'primary_reading' value = 'T'> Soil "
    "temperature ";
WiFiManagerParameter primary_reading_param(primary_reading_radio_str);

const char *moist_radio_str =
    "<br/><label for='moist_format'>Soil moisture reported as:<br/></label>"
    "<input type = 'radio' name = 'moist_format' value = '%' checked /> "
    "Percent "
    "<br/>"
    "<input type = 'radio' name = 'moist_format' value = 'r'> "
    "Raw sensor readings<br/>";
WiFiManagerParameter moist_format_param(moist_radio_str);

WiFiManagerParameter mqtt_broker_address_param("mqtt_broker_address",
                                               "MQTT Broker Address",
                                               "test.mosquitto.org", 50);

WiFiManagerParameter mqtt_broker_port_param("mqtt_broker_port",
                                            "MQTT Broker Port", "1883", 4);

WiFiManagerParameter mqtt_username_param("mqtt_broker_username",
                                         "MQTT username", "", 20);

WiFiManagerParameter mqtt_password_param("mqtt_broker_password",
                                         "MQTT password", "", 20);

WiFiManagerParameter mqtt_topic_param("mqtt_topic", "MQTT topic",
                                      "paper_wifi/test", 20);

void saveParamCallback() {
  Serial.println("[CALLBACK] saveParamCallback fired");
  if (wifi_manager.server->hasArg("primary_reading")) {
    if (wifi_manager.server->arg("primary_reading") ==
        String(static_cast<char>(Settings::PrimaryReading::MOISTURE))) {
      settings.primary_reading = Settings::PrimaryReading::MOISTURE;
    } else {
      settings.primary_reading = Settings::PrimaryReading::TEMPERATURE;
    }
  }
  if (wifi_manager.server->hasArg("temp_format")) {
    if (wifi_manager.server->arg("temp_format") ==
        String(static_cast<char>(Settings::TempFormat::CELSIUS))) {
      settings.temp_format = Settings::TempFormat::CELSIUS;
    } else {
      settings.temp_format = Settings::TempFormat::FARENHEIT;
    }
  }

  if (wifi_manager.server->hasArg("moist_format")) {
    if (wifi_manager.server->arg("moist_format") ==
        String(static_cast<char>(Settings::MoistFormat::RAW))) {
      settings.moist_format = Settings::MoistFormat::RAW;
    } else {
      settings.moist_format = Settings::MoistFormat::PERCENT;
    }
  }

  if (wifi_manager.server->hasArg("mqtt_broker_address")) {
    wifi_manager.server->arg("mqtt_broker_address")
        .toCharArray(settings.mqtt_address, 50, 0);
  }

  if (wifi_manager.server->hasArg("mqtt_broker_port")) {
    settings.mqtt_port = wifi_manager.server->arg("mqtt_broker_port").toInt();
  }

  if (wifi_manager.server->hasArg("mqtt_broker_username")) {
    wifi_manager.server->arg("mqtt_broker_username")
        .toCharArray(settings.mqtt_username, 20, 0);
  }

  if (wifi_manager.server->hasArg("mqtt_broker_password")) {
    wifi_manager.server->arg("mqtt_broker_password")
        .toCharArray(settings.mqtt_password, 20, 0);
  }

  if (wifi_manager.server->hasArg("mqtt_topic")) {
    wifi_manager.server->arg("mqtt_topic")
        .toCharArray(settings.mqtt_topic, 20, 0);
  }
}

void WifiConfigSetup() {
  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // WiFi.mode(WIFI_AP);
  wifi_manager.setAPStaticIPConfig(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1),
                                   IPAddress(255, 255, 255, 0));

  wifi_manager.addParameter(&primary_reading_param);
  wifi_manager.addParameter(&temp_format_param);
  wifi_manager.addParameter(&moist_format_param);
  wifi_manager.addParameter(&mqtt_broker_address_param);
  wifi_manager.addParameter(&mqtt_broker_port_param);
  wifi_manager.addParameter(&mqtt_username_param);
  wifi_manager.addParameter(&mqtt_password_param);
  wifi_manager.addParameter(&mqtt_topic_param);
  wifi_manager.setSaveParamsCallback(saveParamCallback);
  wifi_manager.setConfigPortalTimeout(30);
}