
#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include <EEPROM.h>

#include "sensor_board.h"
#include "web_mqtt.h"
#include "web_overview.h"
#include "web_netsetting.h"



void WriteStringToEEPROM(int beginaddress, String string)
{
	char  charBuf[string.length()+1];
	string.toCharArray(charBuf, string.length()+1);
	for (int t=  0; t<sizeof(charBuf);t++)
	{
		EEPROM.write(beginaddress + t,charBuf[t]);
	}
}

void WriteArrayToEEPROM(int beginaddress,uint8_t *data_w,int n_data_w)
{
    for (int t=  0; t<n_data_w;t++)
	{
		EEPROM.write(beginaddress + t,*(data_w+t));
	}
}

void ReadArrayFromEEPROM(int beginaddress,uint8_t *data_w,int n_data_w)
{
    for (int t=  0; t<n_data_w;t++)
	{
        *(data_w+t)=EEPROM.read(beginaddress + t);
    }
}

String  ReadStringFromEEPROM(int beginaddress)
{
	byte counter=0;
	char rChar;
	String retString = "";
	while (1)
	{
		rChar = EEPROM.read(beginaddress + counter);
		if (rChar == 0) break;
		if (counter > 31) break;
		counter++;
		retString.concat(rChar);

	}
	return retString;
}

void EEPROMWriteInt(int address, uint16_t value)
{
      byte lsb = (value & 0xFF);
      byte msb = ((value >> 8) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, lsb);
      EEPROM.write(address + 1, msb);
}
uint16_t EEPROMReadInt(long address)
{
      //Read the 4 bytes from the eeprom memory.
      long lsb = EEPROM.read(address);
      long msb = EEPROM.read(address + 1);

      //Return the recomposed long by using bitshift.
      return ((lsb<< 0) & 0xFF) + ((msb << 8) & 0xFFFF);
}

void EEPROMWritelong(int address, long value)
      {
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }
long EEPROMReadlong(long address)
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);

      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

boolean ReadConfig()
{
  delay(100);
//	Serial.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
	{
    st01.net.dhcp=EEPROM.read(16);
    // EEPROM.write(16,st01.net.dhcp);
	  // EEPROM.write(17,st01.net.localIp[0]);
    // EEPROM.write(18,st01.net.localIp[0]);
    // EEPROM.write(19,st01.net.localIp[0]);
    // EEPROM.write(20,st01.net.localIp[0]);
    ReadArrayFromEEPROM(17,st01.net.ip,4);
    ReadArrayFromEEPROM(21,st01.net.subnet,4);
    ReadArrayFromEEPROM(25,st01.net.gatway,4);
    ReadArrayFromEEPROM(29,st01.net.dns,4);
    Serial.printf("dhcp:%d\r\n",st01.net.dhcp);
    Serial.printf("ip:%d.%d.%d.%d\r\n",st01.net.ip[0],st01.net.ip[1],st01.net.ip[2],st01.net.ip[3]); 
    Serial.printf("sub:%d.%d.%d.%d\r\n",st01.net.subnet[0],st01.net.subnet[1],st01.net.subnet[2],st01.net.subnet[3]); 
    Serial.printf("gw:%d.%d.%d.%d\r\n",st01.net.gatway[0],st01.net.gatway[1],st01.net.gatway[2],st01.net.gatway[3]); 
    Serial.printf("dns:%d.%d.%d.%d\r\n",st01.net.dns[0],st01.net.dns[1],st01.net.dns[2],st01.net.dns[3]); 
    st01.net.localIp[0]=st01.net.ip[0];
    st01.net.localIp[1]=st01.net.ip[1];
    st01.net.localIp[2]=st01.net.ip[2];
    st01.net.localIp[3]=st01.net.ip[3];

    st01.net.localSubnet[0]=st01.net.subnet[0];
    st01.net.localSubnet[1]=st01.net.subnet[1];
    st01.net.localSubnet[2]=st01.net.subnet[2];
    st01.net.localSubnet[3]=st01.net.subnet[3];

    st01.net.localGateWay[0]=st01.net.gatway[0];
    st01.net.localGateWay[1]=st01.net.gatway[1];
    st01.net.localGateWay[2]=st01.net.gatway[2];
    st01.net.localGateWay[3]=st01.net.gatway[3];

    st01.net.localDns[0]=st01.net.dns[0];
    st01.net.localDns[1]=st01.net.dns[1];
    st01.net.localDns[2]=st01.net.dns[2];
    st01.net.localDns[3]=st01.net.dns[3];

    ReadArrayFromEEPROM(81,(uint8_t *)st01.net.ssid,32);
    ReadArrayFromEEPROM(113,(uint8_t *)st01.net.password,32);

    ReadArrayFromEEPROM(145,(uint8_t *)st01.mqtt.host_name,65);
    st01.mqtt.port=EEPROMReadInt(210);
    ReadArrayFromEEPROM(212,(uint8_t *)st01.mqtt.device_name,33);
    ReadArrayFromEEPROM(245,(uint8_t *)st01.mqtt.Client_id,129);
    ReadArrayFromEEPROM(374,(uint8_t *)st01.mqtt.user,33);
    ReadArrayFromEEPROM(407,(uint8_t *)st01.mqtt.pass,33);
    ReadArrayFromEEPROM(440,(uint8_t *)st01.mqtt.pub,129);
    ReadArrayFromEEPROM(569,(uint8_t *)st01.mqtt.sub,129);

    ReadArrayFromEEPROM(601,(uint8_t *)st01.net.ap_ssid,32);
    ReadArrayFromEEPROM(633,(uint8_t *)st01.net.ap_password,32);
    // EEPROM.write(665,st01.net.ssid_en);
    // EEPROM.write(666,st01.net.ap_ssid_en);
    st01.net.ssid_en=EEPROM.read(665);
    st01.net.ap_ssid_en=EEPROM.read(666);
    st01.mqtt.pub_period=EEPROMReadlong(667);

    ReadArrayFromEEPROM(671,(uint8_t *)st01.mqtt.cert_ca_fs_name,129);
    ReadArrayFromEEPROM(800,(uint8_t *)st01.mqtt.cert_crt_fs_name,129);
    ReadArrayFromEEPROM(929,(uint8_t *)st01.mqtt.cert_pri_fs_name,129);
    st01.mqtt.mqtt_ssl=EEPROM.read(1059);

    Serial.printf("mqtt ssl:%d\r\n",st01.mqtt.mqtt_ssl); 
    Serial.printf("host:%s\r\n",st01.mqtt.host_name); 
    Serial.printf("port:%d\r\n",st01.mqtt.port); 
    Serial.printf("Device:%s\r\n",st01.mqtt.device_name); 
    Serial.printf("Client ID:%s\r\n",st01.mqtt.Client_id); 
    Serial.printf("user:%s\r\n",st01.mqtt.user); 
    Serial.printf("pass:%s\r\n",st01.mqtt.pass); 
    Serial.printf("pub:%s\r\n",st01.mqtt.pub); 
    Serial.printf("sub:%s\r\n",st01.mqtt.sub); 
    Serial.printf("pub_t:%d\r\n",st01.mqtt.pub_period); 
    Serial.printf("cert_ca:%s\r\n",st01.mqtt.cert_ca_fs_name); 
    Serial.printf("cert_crt:%s\r\n",st01.mqtt.cert_crt_fs_name); 
    Serial.printf("cert_pri:%s\r\n",st01.mqtt.cert_pri_fs_name); 
    Serial.printf("===================================\r\n"); 

    Serial.println("STA mode");
    Serial.println(st01.net.ssid_en);
    Serial.printf("ssid:%s\r\n",st01.net.ssid); 
    Serial.printf("pass:%s\r\n",st01.net.password); 
    Serial.println("AP mode");
    Serial.println(st01.net.ap_ssid_en);
    Serial.printf("ssid:%s\r\n",st01.net.ap_ssid); 
    Serial.printf("pass:%s\r\n",st01.net.ap_password); 

	  return true;
	}
	else
	{
		Serial.println("Configurarion NOT FOUND!!!!");
    st01.net.localIp[0]=192;
    st01.net.localIp[1]=168;
    st01.net.localIp[2]=1;
    st01.net.localIp[3]=10;

    st01.net.localSubnet[0]=255;
    st01.net.localSubnet[1]=255;
    st01.net.localSubnet[2]=255;
    st01.net.localSubnet[3]=0;

    st01.net.localGateWay[0]=192;
    st01.net.localGateWay[1]=168;
    st01.net.localGateWay[2]=1;
    st01.net.localGateWay[3]=1;

    st01.net.localDns[0]=8;
    st01.net.localDns[1]=8;
    st01.net.localDns[2]=8;
    st01.net.localDns[3]=8;

    memcpy(st01.net.web_pass,"admin",sizeof("admin"));
    memcpy(st01.net.web_user,"admin",sizeof("admin"));

    st01.net.ap_ssid_en=1;
    memcpy(st01.net.ap_ssid,"Ftot_st01",sizeof("Ftot_st01"));
    memcpy(st01.net.ap_password,"st0123456",sizeof("st0123456"));
    Serial.println("AP SSID");
    Serial.printf("ssid:%s\r\n",st01.net.ap_ssid); 
    Serial.printf("pass:%s\r\n",st01.net.ap_password); 
    
    st01.net.ssid_en=1;
    memcpy(st01.net.ssid,"BANONGLEE_2.4G",sizeof("BANONGLEE_2.4G"));
    memcpy(st01.net.password,"WANVIM27",sizeof("WANVIM27"));
    Serial.println("STA SSID");
    
    Serial.printf("ssid:%s\r\n",st01.net.ssid); 
    Serial.printf("pass:%s\r\n",st01.net.password); 
    
    st01.mqtt.mqtt_ssl=0;
    st01.mqtt.pub_period=10;
		return false;
	}
}

void WriteConfig() {
	Serial.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');

	EEPROM.write(16,st01.net.dhcp);
	EEPROM.write(17,st01.net.ip[0]);
  EEPROM.write(18,st01.net.ip[1]);
  EEPROM.write(19,st01.net.ip[2]);
  EEPROM.write(20,st01.net.ip[3]);

  WriteArrayToEEPROM(21,st01.net.subnet,4);
  WriteArrayToEEPROM(25,st01.net.gatway,4);
  WriteArrayToEEPROM(29,st01.net.dns,4);
  // WriteArrayToEEPROM(33,st01.net.dns2,4);

  WriteArrayToEEPROM(41,(uint8_t *)st01.net.web_user,8);
  WriteArrayToEEPROM(49,(uint8_t *)st01.net.web_pass,8);

  WriteArrayToEEPROM(81,(uint8_t *)st01.net.ssid,32);
  WriteArrayToEEPROM(113,(uint8_t *)st01.net.password,32);

  WriteArrayToEEPROM(145,(uint8_t *)st01.mqtt.host_name,65);
  EEPROMWriteInt(210,st01.mqtt.port);
  WriteArrayToEEPROM(212,(uint8_t *)st01.mqtt.device_name,33);
  WriteArrayToEEPROM(245,(uint8_t *)st01.mqtt.Client_id,129);
  WriteArrayToEEPROM(374,(uint8_t *)st01.mqtt.user,33);
  WriteArrayToEEPROM(407,(uint8_t *)st01.mqtt.pass,33);
  WriteArrayToEEPROM(440,(uint8_t *)st01.mqtt.pub,129);
  WriteArrayToEEPROM(569,(uint8_t *)st01.mqtt.sub,129);
  WriteArrayToEEPROM(601,(uint8_t *)st01.net.ap_ssid,32);
  WriteArrayToEEPROM(633,(uint8_t *)st01.net.ap_password,32);
  EEPROM.write(665,st01.net.ssid_en);
  EEPROM.write(666,st01.net.ap_ssid_en);

  EEPROMWritelong(667,st01.mqtt.pub_period);
  EEPROM.write(1059,st01.mqtt.mqtt_ssl);
  

	EEPROM.commit();
}

void Write_cert_fileName()
{
  Serial.println("Writing file cert");

  WriteArrayToEEPROM(671,(uint8_t *)st01.mqtt.cert_ca_fs_name,129);
  WriteArrayToEEPROM(800,(uint8_t *)st01.mqtt.cert_crt_fs_name,129);
  WriteArrayToEEPROM(929,(uint8_t *)st01.mqtt.cert_pri_fs_name,129);
  EEPROM.commit();
}


