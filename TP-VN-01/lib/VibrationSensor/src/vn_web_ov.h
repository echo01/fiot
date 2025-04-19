#ifndef VN_web_Overview
#define VN_web_Overview

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Ticker.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>

class vn_WebPage {
public:
    vn_WebPage(unsigned long tkinterval);
    File file;
    File root;

    String formatBytes(size_t bytes);
    uint8_t deleteFile(const char* filename);
    void listDir();
    void starting_web();

    // AsyncWebServer server;
    AsyncWebServer server;
    WiFiClientSecure wifiSecureClient;
    PubSubClient Mqttclient;
    bool web_active;

    WiFiClient ethClient;
    PubSubClient mqttClient2;
    void onUnauthorized(AsyncWebServerRequest *request);
private:
    unsigned long   _lastMillis;      // Tracks time for periodic tasks
    unsigned long   _interval;        // Interval for background task
    
};

extern vn_WebPage vn_Web;
#endif