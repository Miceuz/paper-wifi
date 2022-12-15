#include "wifi_config.h"

#include "WiFiManager.h"

const char *temp_radio_str =
    "<br/><label for='temp_format'>Temperature format:<br/></label>"
    "<input type = 'radio' name = 'temp_format' value = 'C' checked /> "
    "Celsius<br/>"
    "<input type = 'radio' name = 'temp_format' value = 'F'> Farenheit ";
WiFiManagerParameter temp_format_param(temp_radio_str);

const char *mqtt_checkbox_str =
    "<input type = 'checkbox' name = 'send_mqtt' value = '1' checked /> "
    "<label for='send_mqtt'>Send readings over MQTT</label><br/>";
WiFiManagerParameter mqtt_checkbox_param(mqtt_checkbox_str);

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
  if (wifi_manager.server->hasArg("temp_format")) {
    if (wifi_manager.server->arg("temp_format") ==
        String(static_cast<char>(Settings::TempFormat::CELSIUS))) {
      settings.temperature = Settings::TempFormat::CELSIUS;
      Serial.println("CELSIUS");
    } else {
      settings.temperature = Settings::TempFormat::FARENHEIT;
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
  wifi_manager.setAPStaticIPConfig(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1),
                                   IPAddress(255, 255, 255, 0));

  wifi_manager.addParameter(&temp_format_param);
  wifi_manager.addParameter(&mqtt_checkbox_param);
  wifi_manager.addParameter(&mqtt_broker_address_param);
  wifi_manager.addParameter(&mqtt_broker_port_param);
  wifi_manager.addParameter(&mqtt_username_param);
  wifi_manager.addParameter(&mqtt_password_param);
  wifi_manager.addParameter(&mqtt_topic_param);
  wifi_manager.setSaveParamsCallback(saveParamCallback);
  wifi_manager.setConfigPortalTimeout(120);
}