#include "wifi_config.h"
#include "WiFiManager.h"

const char *temp_radio_str =
    "<br/><label for='temp_format'>Temperature format:<br/></label>"
    "<input type = 'radio' name = 'temp_format' value = 'C' checked /> "
    "Celsius<br/>"
    "<input type = 'radio' name = 'temp_format' value = 'F'> Farenheit ";
WiFiManagerParameter temp_format_param(temp_radio_str);

const char *moist_radio_str =
    "<br/><label for='moist_format'>Soil moisture reported as:<br/></label>"
    "<input type = 'radio' name = 'moist_format' value = '%' checked /> "
    "Percent "
    "<br/>"
    "<input type = 'radio' name = 'moist_format' value = 'r'> "
    "Raw sensor readings<br/>";
WiFiManagerParameter moist_format_param(moist_radio_str);

const char *mqtt_checkbox_str =
    "<input type = 'checkbox' name = 'send_mqtt' value = '1' checked /> "
    "<label for='send_mqtt'>Send readings over MQTT</label><br/>";
WiFiManagerParameter mqtt_checkbox_param(mqtt_checkbox_str);

WiFiManagerParameter mqtt_broker_address_param("mqtt_broker_address",
                                               "MQTT Broker Address", "", 50);

WiFiManagerParameter mqtt_username_param("mqtt_broker_username",
                                         "MQTT username", "", 20);

WiFiManagerParameter mqtt_password_param("mqtt_broker_password",
                                         "MQTT password", "", 20);

void saveParamCallback() {
  Serial.println("[CALLBACK] saveParamCallback fired");
  if (wifi_manager.server->hasArg("temp_format")) {
    if (wifi_manager.server->arg("temp_format") ==
        String(static_cast<char>(Settings::TempFormat::CELSIUS))) {
      settings.temperature = Settings::TempFormat::CELSIUS;
      Serial.println("CELSIUS");
    } else {
      settings.temperature = Settings::TempFormat::FARENHEIT;
    }
  }

  if (wifi_manager.server->hasArg("moist_format")) {
    if (wifi_manager.server->arg("moist_format") ==
        String(static_cast<char>(Settings::MoistFormat::RAW))) {
      settings.moisture = Settings::MoistFormat::RAW;
      Serial.println("RAW MOISTURE");
    } else {
      settings.moisture = Settings::MoistFormat::PERCENT;
    }
  }

  if (wifi_manager.server->hasArg("send_mqtt")) {
    if (wifi_manager.server->arg("send_mqtt") == "1") {
      settings.send_mqtt = true;
      Serial.println("Send MQTT");
    } else {
      settings.send_mqtt = false;
    }
  }

  if (wifi_manager.server->hasArg("mqtt_broker_address")) {
    settings.mqtt_address = wifi_manager.server->arg("mqtt_broker_address");
  }

  if (wifi_manager.server->hasArg("mqtt_broker_username")) {
    settings.mqtt_username = wifi_manager.server->arg("mqtt_broker_address");
  }

  if (wifi_manager.server->hasArg("mqtt_broker_password")) {
    settings.mqtt_password = wifi_manager.server->arg("mqtt_broker_address");
  }
}

void WifiConfigSetup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  WiFi.mode(WIFI_AP);
  wifi_manager.setAPStaticIPConfig(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1),
                                   IPAddress(255, 255, 255, 0));

  wifi_manager.addParameter(&temp_format_param);
  wifi_manager.addParameter(&moist_format_param);
  wifi_manager.addParameter(&mqtt_checkbox_param);
  wifi_manager.addParameter(&mqtt_broker_address_param);
  wifi_manager.addParameter(&mqtt_username_param);
  wifi_manager.addParameter(&mqtt_password_param);
  wifi_manager.setSaveParamsCallback(saveParamCallback);
  wifi_manager.setConfigPortalTimeout(10);
}