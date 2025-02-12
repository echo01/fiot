#ifndef VN_web_Overview
#define VN_web_Overview

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Ticker.h"
#include <esp_wifi.h>

class vn_page_ov {
public:
    vn_page_ov();
    File file;
    File root;
private:

};

#endif