#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#include <stdbool.h>

#include "Arduino.h"
#include "WiFiManager.h"

extern WiFiManager wifi_manager;
extern bool is_params_saved;
void WifiConfigSetup();
#endif