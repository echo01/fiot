#ifndef mb_master
#define mb_master

#include <Arduino.h>


typedef union {
	uint16_t word;
	struct {
		uint8_t L;
		uint8_t H;
	};
} type_crc;


class master_mb
{
private:
    /* data */
    unsigned long       _lastmillis;
    unsigned long       _interval;
    unsigned long       _time_out;
    unsigned long       _between_poles_t;
    unsigned long       _scan_t;

    unsigned long       loopcnt;
    uint16_t            seq_cmd;
    uint16_t            max_cmd;
    
public:
    HardwareSerial      *serialPtr;
    type_crc            crc;
    uint8_t             ready;
    master_mb(HardwareSerial *ptSerial,uint32_t inv,uint32_t timeout,uint32_t between_poles,uint32_t scan_t);
    ~master_mb();
    void handelTask();
    void run_ex();

    void set_serial(HardwareSerial *ptSerial);
    void set_interval(unsigned long inv);
    void set_timeout(unsigned long timeout);
    void set_between_poles(unsigned long poles_t);
    void set_scan_time(unsigned long scan_t);
    uint16_t CAL_CRC16(type_crc *crcptr, uint8_t *dptr, uint8_t n);
    void set_read_command(uint8_t *mbtable,uint16_t cmd,uint16_t id,uint16_t str_req,uint16_t reg_req);
    // void ConfigSerial(byte datasize, byte stopbit, byte parity,byte baudrate);
};


#endif