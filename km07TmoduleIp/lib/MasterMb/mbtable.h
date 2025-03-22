#ifndef _mdtable
#define _mdtable

#include <Arduino.h>


class mbtable
{
private:
    /* data */

public:
    mbtable(uint16_t sizeof_reg);
    ~mbtable();
    uint16_t    reg[125];    
    uint16_t    reg_size;
    uint16_t    index_update;
    uint8_t     rtu_req_cmd[32];
    uint8_t     rtu_req_cmd_ok;
    uint8_t     rtu_update_cmd[255];
    uint8_t     rtu_update_cmd_ok;
};





#endif