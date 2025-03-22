#include "master_mb.h"
#include "mbtable.h"

#define UART1RX_PIN         5
#define UART1TX_PIN         17

mbtable tb1(125);
mbtable tb2(125);
mbtable tb3(125);
mbtable tb4(125);
mbtable tb5(125);
mbtable tb6(125);
mbtable tb7(125);

const uint8_t CRC_HI[] = { 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
  0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
  0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
  0xC1, 0x81, 0x40 };

const uint8_t CRC_LO[] = { 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05,
  0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A,
  0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B,
  0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14,
  0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11,
  0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36,
  0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF,
  0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28,
  0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D,
  0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22,
  0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63,
  0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C,
  0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69,
  0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE,
  0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77,
  0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50,
  0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55,
  0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A,
  0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B,
  0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44,
  0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41,
  0x81, 0x80, 0x40 };


master_mb::master_mb(HardwareSerial *ptSerial,uint32_t inv,uint32_t timeout,uint32_t between_poles,uint32_t scan_t)
{
  _interval=inv;
  _time_out=timeout;
  _between_poles_t=between_poles;
  _scan_t=scan_t;
  serialPtr=ptSerial;
  // ConfigSerial(d8bits,stopbits1,parity_none,bps115200);
  seq_cmd=0;
}

master_mb::~master_mb()
{

}

uint16_t master_mb::CAL_CRC16(type_crc *crcptr, uint8_t *dptr, uint8_t n) {
	uint8_t i, j;
	crcptr->H = crcptr->L = 0xFF;
	for (i = 0; i < n; i++, dptr++) {
		j = crcptr->H ^ *dptr;
		crcptr->H = crcptr->L ^ CRC_HI[j];
		crcptr->L = CRC_LO[j];
	}
  return(crcptr->word);
}

void master_mb::set_serial(HardwareSerial *ptSerial)
{
  serialPtr=ptSerial;
}

void master_mb::set_interval(unsigned long inv)
{
    _interval=inv;
}

void master_mb::set_timeout(unsigned long timeout)
{
  _time_out=timeout;
}

void master_mb::set_between_poles(unsigned long poles_t)
{
  _between_poles_t=poles_t;
}

void master_mb::set_scan_time(unsigned long scan_t)
{
  _scan_t=scan_t;
}

void master_mb::set_read_command(uint8_t *mbtable,uint16_t cmd,uint16_t id,uint16_t str_req,uint16_t reg_req)
{
  type_crc mb_crc;
  switch (cmd)
  {
  case 3:
  case 4:
    mbtable[0]=id;
    mbtable[1]=cmd;
    mbtable[2]=(str_req>>8)&0x00FF;
    mbtable[3]=(str_req&0x00FF);
    mbtable[4]=(reg_req>>8)&0x00FF;
    mbtable[5]=(reg_req&0x00FF);
    mb_crc.word=CAL_CRC16(&mb_crc,mbtable,6);
    break;
  case 6:
    break;
  case 16:
    break;
  default:
    break;
  }
}

void master_mb::handelTask()
{
    unsigned long currentMills=millis();
    if(currentMills-_lastmillis >= _interval)
    {
        _lastmillis = currentMills;
        loopcnt++;
        serialPtr->println("handel task"+String(loopcnt));
    }
}

void master_mb::run_ex()
{
    if(ready==0)
        {       return;        }
    
}


/*=====================================================================================================
**
** CONFIGURATION SERIAL
**
=====================================================================================================*/
// void master_mb::ConfigSerial(byte datasize, byte stopbit, byte parity,byte baudrate)
// {
//   uint32_t serconfig = SERIAL_8N1;
//   int serbaudrate = 9600;
  
//   if(datasize == 0) {
//     if(stopbit == 1) {
//       switch(parity) {
//         case 0 : serconfig = SERIAL_7N1;
//           break; 
//         case 1 : serconfig = SERIAL_7E1;
//           break; 
//         case 2 : serconfig = SERIAL_7O1;
//           break; 
//       }
//     }else if(stopbit == 2) {
//       switch(parity) {
//         case 0 : serconfig = SERIAL_7N2;
//           break; 
//         case 1 : serconfig = SERIAL_7E2;
//           break; 
//         case 2 : serconfig = SERIAL_7O2;
//           break; 
//       }
//     }
//   }else if(datasize == 1) {
//     if(stopbit == 1) {
//       switch(parity) {
//         case 0 : serconfig = SERIAL_8N1;
//           break; 
//         case 1 : serconfig = SERIAL_8O1;
//           break; 
//         case 2 : serconfig = SERIAL_8E1;
//           break; 
//       }
//     }else if(stopbit == 2) {
//       switch(parity) {
//         case 0 : serconfig = SERIAL_8N2;
//           break; 
//         case 1 : serconfig = SERIAL_8O2;
//           break; 
//         case 2 : serconfig = SERIAL_8E2;
//           break; 
//       }
//     }
//   }

//   switch(baudrate) {
//     case 0 : serbaudrate = 2400; 
//         break; 
//     case 1 : serbaudrate = 4800; 
//       break; 
//     case 2 : serbaudrate = 9600; 
//       break; 
//     case 3 : serbaudrate = 19200; 
//       break; 
//     case 4 : serbaudrate = 38400; 
//       break; 
//     case 5 : serbaudrate = 57600; 
//       break; 
//     case 6 : serbaudrate = 115200; 
//       break; 
//     default :serbaudrate = 9600;
//   }
//   serialPtr->begin(serbaudrate, serconfig,UART1RX_PIN,UART1TX_PIN);
//   // Serial1.begin(serbaudrate, serconfig,UART1RX_PIN,UART1TX_PIN);
//   //Serial1.begin(serbaudrate, serconfig,
// }