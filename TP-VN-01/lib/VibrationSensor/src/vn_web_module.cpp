#include "vn_web_module.h"
#include <EEPROM.h>
#include "vn_web_ov.h"
#include "mems.h"

#define eeprom_size     1256
RTC_DATA_ATTR bool wokeFromDeepSleep = false;

vn_module::vn_module(unsigned long tkinterval,uint16_t port_http):_interval(tkinterval),http_server(port_http) 
{

}

void vn_module::begin()
{
    // EEPROM.begin(eeprom_size);
    String myString = "BANONGLEE_2.4G";
    // String myString = "Park_2.4G";
    // myString = "";
    myString.toCharArray(vn_module::net.ssid,sizeof(net.ssid));
    myString = "WANVIM27";
    // myString = "park6789";
    myString.toCharArray(vn_module::net.ssid_pws,sizeof(net.ssid_pws));

    getMACAddressAsArray(net.mac);
    Serial.print("AP SSID: ");
    
    myString = "vn"+String(net.mac[4],HEX)+String(net.mac[5],HEX);
    // Serial.println(myString);
    myString.toCharArray(vn_module::net.ap_ssid,sizeof(net.ap_ssid));

    myString = "vn@3p"+String(net.mac[4],HEX)+String(net.mac[5],HEX);
    myString.toCharArray(vn_module::net.ap_pws,sizeof(net.ap_pws));

    // check ssip of sta mode
    net.mode=VN_NET_MODE_STA;
    if(net.mode==VN_NET_MODE_STA) // sta mode
    {
        if(strlen(net.ssid)==0)  // start AP mode
            net_status=VN_NET_AP;
        else    // start STA mode
            net_status=VN_NET_STA;
    }
    else
    {
        net.mode=VN_NET_MODE_AP;
        net_status=VN_NET_AP;
    }
    
    net.status=0;
    try_connect=0;
    delay_interval=0;
    _sleepInterval=60000;   //  60 sec.
    taskNetwork.attach_ms(_interval, periodicTaskWrapper, this);
}

void vn_module::getMACAddressAsArray(uint8_t macArray[6]) {
    String macString = WiFi.macAddress();
    int index = 0;
    for (int i = 0; i < macString.length(); i += 3) {
        String byteString = macString.substring(i, i + 2);
        macArray[index++] = (uint8_t) strtol(byteString.c_str(), nullptr, 16);
    }
}

void vn_module::printWiFiStatus() {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Subnet Address: ");
    Serial.println(WiFi.subnetMask());

    Serial.print("GateWay Address: ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("DNS Address: ");
    Serial.println(WiFi.dnsIP());

    Serial.print("MAC Address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
}

void vn_module::periodicTaskWrapper(vn_module* instance) {
    instance->network_loop();
    instance->module_ex(instance);
}

void vn_module::module_ex(vn_module* instance)
{
    unsigned long currentMillis=millis();
    if(currentMillis-instance->_sleepCount>instance->_sleepInterval)
    {       //  go to sleep     //
        instance->_sleepCount=currentMillis;
        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1); // Wake on INT1 HIGH
        Serial.println("Going to sleep. Move to wake...");
        // delay(1000);
        // esp_light_sleep_start();
        // Serial.println("Woke up!");
        // instance->esp_state=0;
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1);  // Wake on HIGH
        esp_deep_sleep_start();
    }
    if(vn_mems.motion_weak_up)
    {   // still weak up, ater mems interrupt active
        vn_mems.motion_weak_up=0;
        instance->_sleepCount=currentMillis;
    }
    if(vn_Web.web_active)
    {   //  still weak up, after web active
        vn_Web.web_active=false;
        instance->_sleepCount=currentMillis;
    }
    if(instance->module_weak_up)
    {   //  alway weak up when module_weak_up is true.
        instance->_sleepCount=currentMillis;
    }
    // switch (instance->esp_state)
    // {
    // case 0:
    // default:        //  weak up
    //     // start network    //
    //     instance->_sleepCount=currentMillis;
    //     // instance->net.status=255;
    //     instance->esp_state=1;
    //     break;
    // case 1:
    // //  weak up
    //     instance->network_loop();
    //     break;
    // }
}

void vn_module::network_loop()
{
    switch (net.status)
    {
    default:
    case 255:   //reconnect
        Serial.print("Disconnecting to WiFi... reconnect..");
        WiFi.disconnect();
    case VN_NET_START: // Start WIFI
        /* code */
        if( (net.mode==VN_NET_MODE_STA)&&(net_status==VN_NET_STA) )
            {   //  start STA
            WiFi.begin(net.ssid, net.ssid_pws);
            Serial.print("Connecting to WiFi : ");
            Serial.println(net.ssid);
            net.status=VN_NET_WAIT_CONNECT;
            delay_interval=0;
            }
        else
            {   // start AP
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAP(net.ap_ssid,net.ap_pws);
            net.apIp = WiFi.softAPIP();
            Serial.print("Access Point IP address: ");
            Serial.println(net.ap_ssid);
            Serial.println(net.apIp);
            net.status=VN_NET_STARTED;
            module_weak_up=true;
            }
            // vn_Web.server.
        break;
    case VN_NET_WAIT_CONNECT: //  wait WIFI connect
        if( (net.mode==VN_NET_MODE_STA)&&(net_status==VN_NET_STA) )
            {
            if(WiFi.status() != WL_CONNECTED) 
                {
                Serial.print(".");
                }
            if(WiFi.status() == WL_CONNECTED) 
                {
                net.status=VN_NET_STARTED;
                module_weak_up=false;
                Serial.println("\nWiFi connected.");
                printWiFiStatus();
                }
            if(delay_interval++>=15)     // delay 15 cycle
                {
                if(try_connect++<=3)
                    {
                    Serial.println("\nFail to connected SSID..restart re-try..");
                    Serial.println(net.ssid);
                    Serial.print("re-try count =");
                    Serial.println(try_connect);
                    }
                else
                    {
                    Serial.println("\nFail to connected SSID..restart with AP mode..");
                    Serial.println(net.ap_ssid);
                    net_status=VN_NET_AP;
                    }
                WiFi.disconnect();
                delay_interval=0;
                net.status=VN_NET_START;
                }
            }

        break;
    case VN_NET_STARTED: //  WIFI Connected start web server
        vn_Web.starting_web();
        net.status=VN_NET_MQTT_START;
        break;
    case VN_NET_MQTT_START:

        break;
    // default:
        // break;
    }
    
}


void vn_module::listConnectedDevices() {
    Serial.println("Connected devices:");
    wifi_sta_list_t stationList;
    tcpip_adapter_sta_list_t adapterList;

    esp_wifi_ap_get_sta_list(&stationList);
    tcpip_adapter_get_sta_list(&stationList, &adapterList);

    for (int i = 0; i < adapterList.num; i++) {
        tcpip_adapter_sta_info_t station = adapterList.sta[i];
        Serial.printf("Device %d: MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                      i + 1,
                      station.mac[0], station.mac[1], station.mac[2],
                      station.mac[3], station.mac[4], station.mac[5]);
    }
}

String vn_module::formatBytes(size_t bytes)
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

