#pragma once

#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#endif
#include <EEPROM.h>

extern void WriteStringToEEPROM(int beginaddress, String string);
extern String  ReadStringFromEEPROM(int beginaddress);
extern void EEPROMWritelong(int address, long value);
extern long EEPROMReadlong(long address);
extern boolean ReadConfig();
extern void WriteConfig();
extern void Write_cert_fileName();
extern void Write_save_webpass();




