#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#include <stdbool.h>

#include "Arduino.h"
#include "WiFiManager.h"

extern WiFiManager wifi_manager;

void WifiConfigSetup();
#endif