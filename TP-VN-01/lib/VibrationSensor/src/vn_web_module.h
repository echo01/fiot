#ifndef VN_web_Module
#define VN_web_Module

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Ticker.h"
#include <esp_wifi.h>
#include "SPIFFS.h"

typedef struct{
    char ssid[32];
    char ssid_pws[32];
    char ap_ssid[32];
    char ap_pws[32];
    IPAddress apIp;
    IPAddress localIp;
    IPAddress localSubnet;
    IPAddress localGateWay;
    IPAddress localDns;
    IPAddress localDns2;
    uint8_t mac[6];
    uint8_t mode;
    uint8_t DHCP;
    long rssi;
    uint8_t status;
}type_network;

typedef struct{
    char     host_name[65];
    uint16_t port;
    char     device_name[33];
    char     Client_id[129];
    char     user[33];
    char     pass[65];
    char     pub[129];
    char     sub[129];
    uint8_t mqtt_ssl;
    uint8_t status;
}type_config_mqtt;

typedef struct{
    char AWS_CERT_CA1[2000];
    char AWS_CERT_CRT1[2000];
    char AWS_CERT_PRIVATE1[2000];
    uint8_t cert_ca_size;
    uint8_t cert_crt_size;
    uint8_t cert_pri_size;
    uint8_t cert_ca_ready;
    uint8_t cert_crt_ready;
    uint8_t cert_pri_ready;
    char    cert_ca_fs_name[129];
    char    cert_crt_fs_name[129];
    char    cert_pri_fs_name[129];
}type_key_mqtt;

typedef struct
{
    uint8_t host_status;
    uint16_t cnt_host_connect;
    uint32_t pub_period;
    uint32_t pub_cnt;
    uint32_t sub_cnt;
}type_mqtt_status;

typedef enum {
    VN_NET_FAIL         = 255,   // for compatibility with WiFi Shield library
    VN_NET_NONE_STA     = 0,
    VN_NET_STA     = 1,
    VN_NET_AP           = 2,
    VN_NET_CONNECTED        = 3,
    VN_NET_CONNECT_FAILED   = 4,
    VN_NET_CONNECTION_LOST  = 5,
    VN_NET_DISCONNECTED     = 6
} vn_net_status;

typedef enum{
    VN_NET_START                = 0,
    VN_NET_WAIT_CONNECT         = 1,
    VN_NET_STARTED              = 2,
    VN_NET_MQTT_START           = 3,
    VN_NET_MQTT_WAIT_CONNECT    = 4,
    VN_NET_MQTT_CONNECTED       = 5,
    VN_NET_MQTT_SEND            = 6,
    VN_NET_MQTT_NOT_CONNECT     = 7,
    VN_NET_MQTT_RE_CONNECT      = 8
} vn_net_state;

typedef enum{
    VN_NET_MODE_AP =0,
    VN_NET_MODE_STA =1
} vn_net_mode;

class vn_module {
public:
    vn_module(unsigned long tkinterval,uint16_t port_http);  // Constructor to set the pin
    void begin();          // Initialize the pin
    static void periodicTaskWrapper(vn_module* instance);
    void network_loop();
    void getMACAddressAsArray(uint8_t macArray[6]);
    void listConnectedDevices();

    String formatBytes(size_t bytes);

    type_network        net;
    type_network        web_setting;
    type_config_mqtt    mqtt_setting;
    type_key_mqtt       mqtt_key;
    type_mqtt_status    mqtt_status;
    uint8_t         net_status;
    File file;
    File root;
private:

    unsigned long   _lastMillis;      // Tracks time for periodic tasks
    unsigned long   _interval;        // Interval for background task
    
    
    AsyncWebServer http_server;
    WiFiClientSecure mqtt_ssl;
    PubSubClient mqtt_client_Secure;

    WiFiClient mqtt_none_ssl;
    PubSubClient mqtt_client;

    uint16_t delay_interval;
    uint16_t try_connect;
    Ticker taskNetwork;

    void printWiFiStatus();
};

#endif