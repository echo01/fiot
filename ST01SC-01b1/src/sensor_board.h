#pragma once

#define _ETHERNET_WEBSERVER_LOGLEVEL_       1
#include "Ticker.h"
#include "Adafruit_SGP40.h"
#include "Adafruit_SHT31.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "PubSubClient.h"
#include "WiFiClientSecure.h"
#include "SPIFFS.h"

#define SAMPLE_RATE 800
// #define SAMPLE_RATE 600
#define NUM_SAMPLES 512   // Number of samples to collect
#define FIR_ORDER 33     // Order of the FIR filter
/*
These values can be changed in order to evaluate the functions
*/
const uint16_t mems_samples = NUM_SAMPLES; //This value MUST ALWAYS be a power of 2
// const double signalFrequency = SAMPLE_RATE;
const double samplingFrequency = SAMPLE_RATE;
const uint8_t amplitude = 100;
const uint16_t samples = 97; //This value MUST ALWAYS be a power of 2

typedef struct{
    uint8_t mac[6];
    uint8_t dhcp;
    uint8_t ip[4];
    uint8_t subnet[4];
    uint8_t gatway[4];
    uint8_t dns[4];
    uint8_t dns2[4];
    char web_user[8];
    char web_pass[8];

    uint8_t ssid_en;
    uint8_t ap_ssid_en;
    char ssid[32];
    char password[32];
    char ap_ssid[32];
    char ap_password[32];
    IPAddress apIp;
    IPAddress localIp;
    IPAddress localSubnet;
    IPAddress localGateWay;
    IPAddress localDns;
    
    String scan_ssid[12];
    int32_t scan_rssi[12];
    wifi_auth_mode_t scan_encryptionType[12];
    int32_t scan_ch[12];
    int scan_num_ssid;

    uint8_t wifiSTA_status;
    uint8_t wifiAp_status;
    long rssi;
}type_network_conf;

typedef struct
{
    char     host_name[65];
    uint16_t    port;
    char     device_name[33];
    char     Client_id[129];
    char     user[33];
    char     pass[65];
    char     pub[129];
    char     sub[129];
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

    uint8_t host_status;
    uint16_t cnt_host_connect;
    uint32_t pub_period;
    uint8_t mqtt_ssl;
    uint32_t pub_cnt;
    uint32_t sub_cnt;
}type_mqtt_conf;

typedef struct{
    uint8_t mqtt_connection_err;
    int8_t mqtt_Fail_rc_err;
    uint8_t no_interret;
}type_err_log;

typedef struct{
  float gReal[NUM_SAMPLES];
  float gImag[NUM_SAMPLES];
  float Filter[NUM_SAMPLES];
  float Velocity[NUM_SAMPLES];  
  float gFFT[NUM_SAMPLES];

  double sumSqrVelocity;
  double RmsVelocity;
  double sumsqrG;
  double RmsG;
  int32_t Calib;
  int8_t OFST;
  float ADJ_OFST;
  float peakFreq;
  float peakValue;
  float Gain_velocity;
  int ipeak_Freq;
}type_axis_data;

typedef struct {
  type_axis_data x;
  type_axis_data y;
  type_axis_data z;
  double roll;    // euler anble name of  x 
  double pitch;   // euler anble name of  y
  double yaw;     // euler anble name of  z
  int index;
  unsigned long tick_task;
  unsigned long sample_tick;
  unsigned long sample_t;
  int CAL_RDY;
  void (*read_mems_task)(void);
  void (*initial_mems_task)(void);
  void (*calibrate_chip_OSFT)(void);
  void (*calibrate_pg_OSFT)(void);
}virbration_mems;

typedef struct 
{
    /* data */
    float temperature;
    float humidity;
    uint16_t voc_raw;
    int32_t voc_index;
    uint16_t raw_adc;
    float Vbat;
    int16_t mems_sampling_rage;
    int16_t mems_g_range;
    float mems_task_period;
    int16_t mems_cmd;
    // int16_t Vbb;
    unsigned long tick_ms_airsensor;
    void (*initial_adcBattery)(void);
    void (*read_task)(void);
    void (*init_adxl345_module)(void);
    void (*read_adxl345_task)(void);

    void (*pub_task)(void);
    void (*sub_task)(void);
    String mac;
    char msg[128];
    type_network_conf net;
    type_mqtt_conf mqtt;
    type_err_log err_log;
} type_st01;

typedef struct {
    float temperature; 
    float humidity;
    int32_t voc_index;
    float Vbat;
    long rssi;
}type_mqtt_value;

extern Adafruit_SGP40 sgp;
extern Adafruit_SHT31 sht31;
extern type_st01 st01;
extern type_mqtt_value mqtt_node;
extern uint32_t tick_Aws_pub;

extern File file;
extern File root;
extern virbration_mems mems_data;

// extern AsyncWebServer server;
extern void Task_st01_read();
extern void initial_adc_vbat();
extern void Task_read_mems();
// extern server from main.cpp
extern AsyncWebServer server;

extern uint8_t deleteFile(const char* filename);
extern void listDir();