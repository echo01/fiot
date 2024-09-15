#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include <EEPROM.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "Adafruit_SGP40.h"
#include "Adafruit_SHT31.h"
#include "PubSubClient.h"
#include "Ticker.h"
#include "WiFiClientSecure.h"
#include "SPIFFS.h"

#include "sensor_board.h"
#include "web_mqtt.h"
#include "web_overview.h"
#include "web_netsetting.h"
// #include "search.h"
#include "memory_op.h"

Adafruit_SGP40 sgp;
Adafruit_SHT31 sht31;
Ticker Task_Sensor;
Ticker Task_Adxl345;
Ticker Aws_pub;
Ticker Mqtt_pub;
type_st01 st01sc;
uint32_t tick_Aws_pub;

char msg[100];
float angleX = 0;
float angleY = 0;
float angleZ = 0;

const char* host = "ESPLINK";
const char* ssid = "BANONGLEE_2.4G";
const char* password = "WANVIM27";

const char* ap_ssid = "Fiot_st01";
const char* ap_password = "st0123456";
int cntSTA_wifi;

// const char* ssid = "Park_2.4G";
// const char* password = "park6789";

const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "2581c443-5a91-4d9f-b406-018e858652d3";
const char* mqtt_username = "7RM7id9wxwxBwJAnMRqrMyPAtmmdWYKq";
const char* mqtt_password = "sG3nJ5rWsZJwWSzPbw8TmR6NaVdhycy9";
const char *www_username = "admin";
const char *www_password = "admin";

const String f_cert_pem="/keys/cert_pem.crt";
const String f_private_key="/keys/private_pem.key";
const String f_cert_ca="/keys/cert_ca.pem";
// const String f_private_key="private_pem.key";

#define AWS_IOT_PUBLISH_TOPIC   "viot/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "viot/sub"

#define THINGNAME "viot01"                         //change this
const char AWS_IOT_ENDPOINT[] = "a139oq97p2bh91-ats.iot.us-east-1.amazonaws.com";       //change this
size_t totalSize = 0;
File file;
File root;

boolean task_mqtt_ready;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

WiFiClient ethClient;
PubSubClient client2(ethClient);

//format bytes
String formatBytes(size_t bytes)
{
  if (bytes < 1024)
  {
    return String(bytes) + "B";
  }
  else if (bytes < (1024 * 1024))
  {
    return String(bytes / 1024.0) + "KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  else
  {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

// put function declarations here:
void listDir()
{
  // File root = SPIFFS.open("/");
  // File file = root.openNextFile();
  root = SPIFFS.open("/");
  file = root.openNextFile();
  size_t fileSize;
  String fileName;
  size_t bytesRead;
  while (file)
  {
    fileName = file.name();
    fileSize = file.size();
    Serial.printf("FS File: %s, size: %s path:%s\n", fileName.c_str(), formatBytes(fileSize).c_str(),file.path() );
    file = root.openNextFile();
  }
  // open file Cert //
  st01.mqtt.cert_ca_ready=1;
  st01.mqtt.cert_crt_ready=1;
  st01.mqtt.cert_pri_ready=1;
  //1.

  // file = SPIFFS.open(f_cert_ca, "r");
  file = SPIFFS.open(st01.mqtt.cert_ca_fs_name, "r");
  if (!file) {
    Serial.println("Failed to open file f_cert_ca for reading");
    st01.mqtt.cert_ca_ready=0;
    return;
  }
  fileName = file.name();
  fileSize = file.size();
  if(fileSize==0)
    st01.mqtt.cert_ca_ready=0;
  bytesRead = file.readBytes(st01.mqtt.AWS_CERT_CA1, fileSize);
  st01.mqtt.AWS_CERT_CA1[bytesRead+1]='\0';
  st01.mqtt.cert_ca_size=strlen(st01.mqtt.AWS_CERT_CA1);
  file.close();
  //2.

  file = SPIFFS.open(st01.mqtt.cert_crt_fs_name, "r");
  if (!file) {
    Serial.println("Failed to open file f_cert_pem for reading");
    st01.mqtt.cert_crt_ready=0;
    return;
  }
  fileName = file.name();
  fileSize = file.size();
  if(fileSize==0)
    st01.mqtt.cert_crt_ready=0;
  bytesRead = file.readBytes(st01.mqtt.AWS_CERT_CRT1, fileSize);
  st01.mqtt.AWS_CERT_CRT1[bytesRead+1]='\0';
  st01.mqtt.cert_crt_size=strlen(st01.mqtt.AWS_CERT_CRT1);
  file.close();
  //3.
  file = root.openNextFile();
  // file3 = SPIFFS.open(f_private_key, "r");
  file = SPIFFS.open(st01.mqtt.cert_pri_fs_name, "r");
  if (!file) {
    Serial.println("Failed to open file f_private_key for reading");
    st01.mqtt.cert_pri_ready=0;
    return;
  }
  fileName = file.name();
  fileSize = file.size();
  if(fileSize==0)
    st01.mqtt.cert_pri_ready=0;
  bytesRead = file.readBytes(st01.mqtt.AWS_CERT_PRIVATE1, fileSize);
  st01.mqtt.AWS_CERT_PRIVATE1[bytesRead+1]='\0';
  st01.mqtt.cert_pri_size=strlen(st01.mqtt.AWS_CERT_PRIVATE1);
  file.close();
  Serial.println();

  Serial.println("1. AWS_CERT_CA ========");
  Serial.println(st01.mqtt.AWS_CERT_CA1);
  Serial.println(st01.mqtt.cert_ca_size);

  Serial.println("2. AWS_CERT_CRT========");
  Serial.println(st01.mqtt.AWS_CERT_CRT1);
  Serial.println(st01.mqtt.cert_crt_size);

  Serial.println("3. AWS_CERT_PRIVATE=====");
  Serial.println(st01.mqtt.AWS_CERT_PRIVATE1);
  Serial.println(st01.mqtt.cert_pri_size);

}

uint8_t deleteFile(const char* filename) {
  if (SPIFFS.exists(filename)) {
    if (SPIFFS.remove(filename)) {
      Serial.println("File deleted successfully");
      return 0;
    } else {
      Serial.println("Error deleting file");
      return 1;
    }
  } else {
    Serial.println("File does not exist");
    return 2;
  }
}

String processor(const String& var){

  return String();
}

void onUnauthorized(AsyncWebServerRequest *request) {
  if (!request->authenticate(st01.net.web_user, st01.net.web_pass))
    return request->requestAuthentication();
}

void publishMessage()
{
  if(tick_Aws_pub++<st01.mqtt.pub_period)
    return;
  tick_Aws_pub=0;
  if(!st01.mqtt.host_status)
  {
    Serial.println("Not ready for pub");
    return;
  }
  Serial.println("Public Data");
  
  StaticJsonDocument<256> doc;
  JsonObject data = doc.createNestedObject("data");
  // StaticJsonDocument<1024> *doc = new StaticJsonDocument<1024>;
  // JsonObject data = doc->createNestedObject("data");
  data["model"] = "vibration_analize";
  data["version"] = "1.00";
  data["rssi"] = mqtt_node.rssi;
  data["Vbat"] = mqtt_node.Vbat;
  data["xV"] = mems_data.x.RmsVelocity*1000;
  data["yV"] = mems_data.y.RmsVelocity*1000;
  data["zV"] = mems_data.z.RmsVelocity*1000;
  data["xg"] = mems_data.x.RmsG;
  data["yg"] = mems_data.y.RmsG;
  data["zg"] = mems_data.z.RmsG;
  data["xPeakHz"] = mems_data.x.peakFreq;
  data["yPeakHz"] = mems_data.y.peakFreq;
  data["zPeakHz"] = mems_data.z.peakFreq;

  // Create a JSON array for the 256 data points (example: xReal data)
  // JsonArray xDataArray = data.createNestedArray("xData");
  // for (int i = 0; i < 256; i++) {
  //   xDataArray.add(String(mems_data.x.gFFT[i], 2).toFloat()); // Add x-axis data to the array
  // }

  // // Similarly, you can create more arrays for yData, zData if needed
  // JsonArray yDataArray = data.createNestedArray("yData");
  // for (int i = 0; i < 256; i++) {
  //   yDataArray.add(String(mems_data.y.gFFT[i], 2).toFloat()); // Add y-axis data
  // }

  // JsonArray zDataArray = data.createNestedArray("zData");
  // for (int i = 0; i < 256; i++) {
  //   zDataArray.add(String(mems_data.z.gFFT[i], 2).toFloat()); // Add z-axis data
  // }

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  // serializeJson(*doc, jsonBuffer);
  

  client.publish(st01.mqtt.pub, jsonBuffer);
  Serial.println(st01.mqtt.pub);
  Serial.println(jsonBuffer);
  st01.mqtt.pub_cnt++;
  // delete doc;  // Free the heap memory
}
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
  st01.mqtt.sub_cnt++;
}

void connectAWS()
{
  if(st01.net.wifiSTA_status==0)
  {
    Serial.println("STA WIFI don't started.");
    st01.err_log.no_interret=1;
    return;
  }
  st01.err_log.no_interret=0;

  net.setCACert(st01.mqtt.AWS_CERT_CA1);
  net.setCertificate(st01.mqtt.AWS_CERT_CRT1);
  net.setPrivateKey(st01.mqtt.AWS_CERT_PRIVATE1);
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  // client.setServer(AWS_IOT_ENDPOINT, 8883);
  client.setServer(st01.mqtt.host_name, st01.mqtt.port);
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
  
  st01.mqtt.host_status=1;
  while (!client.connect(st01.mqtt.Client_id))
  {
    Serial.print(".");
    if(st01.mqtt.cnt_host_connect++>60)
      {
        st01.mqtt.host_status=0;
        st01.err_log.mqtt_Fail_rc_err=client.state();
        break;
      }
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    st01.err_log.mqtt_connection_err=1;
    st01.err_log.mqtt_Fail_rc_err=client.state();
    return;
  }
 
  // Subscribe to a topic
  //client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  client.subscribe(st01.mqtt.sub);
  Serial.println("AWS IoT Connected!");
  Serial.printf("Subcribe:%s\r\n",st01.mqtt.sub);
  st01.err_log.mqtt_connection_err=0;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message received:");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqtt_reconnect() {

  while (!client2.connected()) {
    st01.mqtt.host_status=1;
    Serial.println("Connecting to MQTT broker...");
    if (client2.connect(st01.mqtt.Client_id, st01.mqtt.user, st01.mqtt.pass)) {
    // if (client2.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      // client.subscribe("@shadow/data/update");
      client2.subscribe(st01.mqtt.sub);
      st01.err_log.mqtt_connection_err=0;
    } else {
      st01.mqtt.host_status=0;
      Serial.print("Failed, rc=");
      st01.err_log.mqtt_Fail_rc_err=client.state();
      Serial.print(st01.err_log.mqtt_Fail_rc_err);
      Serial.println(" Retrying in 5 seconds...");
      st01.err_log.mqtt_connection_err=1;
      
      delay(5000);
    }
  }
}

void publishData() {
  task_mqtt_ready=true;
  if(tick_Aws_pub++<st01.mqtt.pub_period)
    return;
  tick_Aws_pub=0;
  if(!st01.mqtt.host_status)
  {
    Serial.println("Not ready for pub");
    return;
  }
  Serial.println("Public Data");
  StaticJsonDocument<200> doc;
  JsonObject data = doc.createNestedObject("data");
  data["Vbat"] = mqtt_node.Vbat;
  data["rssi"] = mqtt_node.rssi;

  data["Xvelocity"] = mems_data.x.RmsVelocity;
  data["Yvelocity"] = mems_data.y.RmsVelocity;
  data["Zvelocity"] = mems_data.z.RmsVelocity;
  data["x_g"] = mems_data.x.RmsG;
  data["y_g"] = mems_data.y.RmsG;
  data["z_g"] = mems_data.z.RmsG;
  data["x_Peak_vibration_Hz"] = mems_data.x.peakFreq;
  data["y_Peak_vibration_Hz"] = mems_data.y.peakFreq;
  data["z_Peak_vibration_Hz"] = mems_data.z.peakFreq;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
  // client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  client2.publish(st01.mqtt.pub, jsonBuffer);
  Serial.println(st01.mqtt.pub);
  Serial.println(jsonBuffer);
  st01.mqtt.pub_cnt++;
}

void Start_WIFI_AP_mode()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(st01.net.ap_ssid, st01.net.ap_password);
  // IPAddress apIP = WiFi.softAPIP();
  st01.net.apIp = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(st01.net.ap_ssid);
  Serial.println(st01.net.apIp);
  // st01.net.ip[0]=st01.net.localIp._address.bytes[0];
  Serial.print("WIF mac: ");
  st01.mac=WiFi.macAddress();
  WiFi.macAddress().toCharArray(st01.msg,st01.mac.length()+1);
  Serial.printf("%s\r\n",st01.msg);
}

void Start_WIFI_AP_STA_mode()
{
  if(st01.net.dhcp==0)
  {
    //  static ip
     WiFi.config(st01.net.localIp, st01.net.localGateWay, st01.net.localSubnet, st01.net.dns);
    Serial.println("Static IP mode");
  }
  else
  {
    Serial.println("DHCP Client mode");
  }
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(st01.net.ap_ssid, st01.net.ap_password);
  WiFi.begin(st01.net.ssid, st01.net.password);
  Serial.println("");
  IPAddress apIP = WiFi.softAPIP();
  st01.net.wifiSTA_status=1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(cntSTA_wifi++>20)
    {
      st01.net.wifiSTA_status=0;
      break;
    }
  }
  Serial.print("Access Point IP address: ");
  Serial.println(st01.net.ssid);
  Serial.println(apIP);
  Serial.print("WIF mac: ");
  st01.mac=WiFi.macAddress();
  WiFi.macAddress().toCharArray(st01.msg,st01.mac.length()+1);
  Serial.printf("%s\r\n",st01.msg);
  // Serial.println(sizeof(WiFi.localIP()));
  st01.net.localIp=WiFi.localIP();
  st01.net.localSubnet= WiFi.subnetMask();
  st01.net.localGateWay = WiFi.gatewayIP();
  st01.net.localDns = WiFi.dnsIP();
  Serial.print("local IP:");
  Serial.println(WiFi.localIP());
  Serial.print("SUB IP:");
  Serial.println(WiFi.subnetMask());
  Serial.print("GW IP:");
  Serial.println(WiFi.gatewayIP());
}

void Start_WIFI_STA_mode()
{
  if(st01.net.dhcp==0)
  {
    //  static ip
     WiFi.config(st01.net.localIp, st01.net.localGateWay, st01.net.localSubnet, st01.net.dns);
    Serial.println("Static IP mode");
  }
  else
  {
    Serial.println("DHCP Client mode");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(st01.net.ssid, st01.net.password);
  Serial.println("");
  IPAddress apIP = WiFi.softAPIP();
  st01.net.wifiSTA_status=1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(cntSTA_wifi++>20)
    {
      st01.net.wifiSTA_status=0;
      break;
    }
  }
  Serial.print("Access Point IP address: ");
  Serial.println(st01.net.ssid);
  Serial.println(apIP);
  Serial.print("WIF mac: ");
  st01.mac=WiFi.macAddress();
  WiFi.macAddress().toCharArray(st01.msg,st01.mac.length()+1);
  Serial.printf("%s\r\n",st01.msg);
  // Serial.println(sizeof(WiFi.localIP()));
  st01.net.localIp=WiFi.localIP();
  st01.net.localSubnet= WiFi.subnetMask();
  st01.net.localGateWay = WiFi.gatewayIP();
  st01.net.localDns = WiFi.dnsIP();
  Serial.print("local IP:");
  Serial.println(WiFi.localIP());
  Serial.print("SUB IP:");
  Serial.println(WiFi.subnetMask());
  Serial.print("GW IP:");
  Serial.println(WiFi.gatewayIP());
}

void Start_WIFI_FN()
{
  // Setup Access Point
  Serial.print("WIF mac: ");
  st01.mac=WiFi.macAddress();
  WiFi.macAddress().toCharArray(st01.msg,st01.mac.length()+1);
  Serial.printf("%s\r\n",st01.msg);
  if(st01.net.ap_ssid_en)
  {
    if(st01.net.ssid_en==0)
    {
      Serial.println("WIFI AP mode enable");
      Start_WIFI_AP_mode();
    }
    else
    {
      Serial.println("WIFI STA mode enable & Enable AP mode");
      Start_WIFI_AP_STA_mode();
    }
  }
  else
  {
    if(st01.net.ssid_en==0)
    {
      Serial.println("WIFI STA mode disable, Force Enable AP mode");
      Start_WIFI_AP_STA_mode();
    }
    else
    {
      Serial.println("WIFI STA mode enable");
      Start_WIFI_STA_mode();
    }
  }
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  EEPROM.begin(1256);
  if (!ReadConfig())
  {
    Serial.println("load factory default");
  }
  st01.mqtt.AWS_CERT_CA1[0]='\0';
  st01.mqtt.AWS_CERT_CRT1[0]='\0';
  st01.mqtt.AWS_CERT_PRIVATE1[0]='\0';

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  listDir();


  Serial.println("................");
  Serial.println("ADXL_345 SENSOR");
  delay(1000);
  Wire.begin();
  Wire.setClock(400000); // Set I2C clock speed to 400 kHz
  // Wire.setClock(200000);
#if 0
  if (! sgp.begin()){
    Serial.println("SGP40 sensor not found :(");
    while (1);
  }

  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1);
  }


  Serial.print("Found SHT3x + SGP40 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
  Task_Sensor.attach(2,st01.read_task);
#else
  st01.initial_adcBattery();
  Task_Sensor.attach(60,st01.read_task);
  st01.init_adxl345_module();
  Task_Adxl345.attach_ms(st01.mems_task_period,st01.read_adxl345_task);

#endif

  // Setup Access Point
  Start_WIFI_FN();
 
  server.on("/css/w3.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/w3.css", "text/css");
  });

  server.on("/css/iotadd.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/iotadd.css", "text/css");
  });

  server.on("/css/fontawesome.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/fontawesome.css", "text/css");
  });

  server.on("/fonts/fontawesome-webfont.ttf", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fonts/fontawesome-webfont.ttf", "font/ttf");
});

  server.on("/assets/Fiot.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/assets/Fiot.png", "image/png");
  });

  server.on("/assets/Fiot2.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/assets/Fiot2.png", "image/png");
  });

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    onUnauthorized(request);
    request->send_P(200, "text/html", web_overview);
  });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    onUnauthorized(request);
    request->send_P(200, "text/html", web_overview);
  });
  api_get_overview();

  //=============== Web page mqtt setting ========================//
  server.on("/mqtt_setting.html", HTTP_GET, [](AsyncWebServerRequest *request){
    onUnauthorized(request);
    request->send_P(200, "text/html", web_mqtt_setting);
  });
  init_apt_web_mqtt_setting();
  api_get_web_mqtt_setting();
  api_get_web_mqtt_file_ca();
  api_get_web_mqtt_file_crt();
  api_get_web_mqtt_file_private();
  //============== Upload Cert File ===========================//
  upload_cert_ca_file();
  upload_cert_crt_file();
  upload_file_private_file();
  
  //=============== Web page network setting ========================//
  server.on("/network_setting.html", HTTP_GET, [](AsyncWebServerRequest *request){
    onUnauthorized(request);
    request->send_P(200, "text/html", web_net_setting);
  });
  //------ Api for Page Network Setting ----------///
  api_get_web_net_setting();
  api_scan_setting();
  init_apt_web_ssid_setting();
  init_apt_ap_web_ssid_setting();
  init_apt_post_net_config();
  init_apt_post_admin_config();
  //------ Api for Page Network Setting ----------///

  // Route to get current angles
  // server.on("/angles", HTTP_GET, [](AsyncWebServerRequest *request){
  //   String json;
  //   StaticJsonDocument<200> doc;
  //   doc["angleX"] = mems_data.roll;
  //   doc["angleY"] = mems_data.pitch;
  //   doc["angleZ"] = mems_data.yaw;
  //   serializeJson(doc, json);
  //   request->send(200, "application/json", json);
  // });
  api_get_angle();
  api_get_fft_chart();
  


  server.begin();
  // connect mqtt mode
  if(st01.mqtt.mqtt_ssl==1)
  {
  connectAWS();
  Aws_pub.attach(1,publishMessage);
  }
  else
  {
    client2.setServer(st01.mqtt.host_name, st01.mqtt.port);
    // Set callback function to handle incoming messages
    mqtt_reconnect();
    client2.setCallback(callback);
    Mqtt_pub.attach(1,publishData);
  }
  
  // Task_Sensor.attach(5,publishMessage);
}

unsigned long previousMillis = 0;
unsigned long interval = 30000;

void loop() {
  // put your main code here, to run repeatedly:
  mqtt_node.humidity=st01.humidity;
  mqtt_node.temperature=st01.temperature;
  mqtt_node.voc_index=st01.voc_index;
  st01.net.rssi=WiFi.RSSI();

  if(st01.mqtt.mqtt_ssl==0)
  {
    if (!client2.connected()) 
    {
    mqtt_reconnect();
    }
  else{
    if(!task_mqtt_ready)
      {
        Serial.println(" Mqtt Start...");
        task_mqtt_ready=true;
        Mqtt_pub.attach(1,publishData);
      }
    }
    client2.loop();
  }
  else
  {
    if (!client.connected()) 
    {
    connectAWS();
    }
    else
    {
    if(!task_mqtt_ready)
      {
        Serial.println(" Mqtt Start...");
        task_mqtt_ready=true;
        Aws_pub.attach(1,publishMessage);
      }
    }
    client.loop();
  }
  
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

  angleX += 1;
  angleY += 2;
  angleZ += 3;

  if (angleX >= 360) angleX = 0;
  if (angleY >= 360) angleY = 0;
  if (angleZ >= 360) angleZ = 0;

  delay(1000); // Simulate a delay between updates
  
}

// put function definitions here:
