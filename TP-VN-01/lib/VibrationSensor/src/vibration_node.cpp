#include "vibration_node.h"

vn_led_status::vn_led_status(uint8_t hb,uint8_t net,uint8_t mqtt,uint8_t link,unsigned long tkinterval):_interval(tkinterval) 
{
    status_io.pin_hb.pin=hb;
    status_io.pin_hb.status=0;

    status_io.pin_link.pin=link;
    status_io.pin_link.status=0;

    status_io.pin_mqtt.pin=mqtt;
    status_io.pin_mqtt.status=0;

    status_io.pin_net.pin=net;
    status_io.pin_net.status=0;

}

void vn_led_status::begin() {
    pinMode(status_io.pin_hb.pin, OUTPUT);
    pinMode(status_io.pin_link.pin,OUTPUT);
    pinMode(status_io.pin_net.pin, OUTPUT);
    pinMode(status_io.pin_mqtt.pin,OUTPUT);
    
    ledoff(&status_io.pin_hb); 
    ledoff(&status_io.pin_link); 
    ledoff(&status_io.pin_net); 
    ledoff(&status_io.pin_mqtt); 
}

void vn_led_status::ledtoggle(type_pin_status *io_pin) {
    io_pin->status=!io_pin->status;
    digitalWrite(io_pin->pin, io_pin->status);
    
}

void vn_led_status::ledon(type_pin_status *io_pin) {
    digitalWrite(io_pin->pin, HIGH);
    io_pin->status=1;
}

void vn_led_status::ledoff(type_pin_status *io_pin) {
    digitalWrite(io_pin->pin, LOW);
    io_pin->status=0;
}

void vn_led_status::handleTaskStatus()
{
     unsigned long currentMillis = millis();
    if (currentMillis - _lastMillis >= _interval) {
        _lastMillis = currentMillis;
        performAction();
    }
}

void vn_led_status::Do_led_hb_normal()
{
    if(vn_led_status::hb_interval>2)    // led hb off after 2 tick
        {
        ledoff(&status_io.pin_hb);
        }

    if(vn_led_status::hb_interval++<100)    // led hb toggle every 100 tick
        return;
    vn_led_status::hb_interval=0;
    ledtoggle(&status_io.pin_hb);
}

void vn_led_status::Do_led_hb_wait_SSID()
{
    if(vn_led_status::hb_interval++<5)    // led hb toggle every 10 tick
        return;
    vn_led_status::hb_interval=0;
    ledtoggle(&status_io.pin_hb);
}

void vn_led_status::Do_led_hd_net_err()
{
    vn_led_status::hb_interval++;
    switch(vn_led_status::hb_interval)
    {
        case 0:
        default:
            ledoff(&status_io.pin_hb);
            break;
        case 1:
        case 2:
        case 10:
        case 11:
            ledon(&status_io.pin_hb);
            break;
    }
    if(vn_led_status::hb_interval<30)    // led hb toggle every 30 tick
        return;
    vn_led_status::hb_interval=0;
}

void vn_led_status::set_led_status_net_err()
{
    vn_led_status::status=3;
}

void vn_led_status::set_led_status_normal()
{
    vn_led_status::status=1;
}

void vn_led_status::set_led_status_wait_network()
{
    vn_led_status::status=2;
}

void vn_led_status::Do_led_mems_done()
{
    if(vn_led_status::mems_interval>2)    // led hb off after 2 tick
        {
        ledoff(&status_io.pin_link);
        }

    if(vn_led_status::mems_interval++<10)    // led hb toggle every 10 tick
        return;
    vn_led_status::mems_interval=0;
    vn_led_status::mems_done=0;
    ledtoggle(&status_io.pin_link);
}

void vn_led_status::set_led_mems_done()
{
    vn_led_status::mems_done=1;
}

void vn_led_status::Do_led_mqtt_normal()
{
    if(vn_led_status::mqtt_interval>2)    // led hb of after 2 tick
        {
        ledoff(&status_io.pin_mqtt);
        }

    if(vn_led_status::mqtt_interval++<100)    // led hb toggle every 100 tick
        return;
    vn_led_status::mqtt_interval=0;
    ledtoggle(&status_io.pin_mqtt);
}

void vn_led_status::Do_led_mqtt_wait_connection()
{

    if(vn_led_status::mqtt_interval++<30)    // led hb toggle every 30 tick
        return;
    vn_led_status::mqtt_interval=0;
    ledtoggle(&status_io.pin_mqtt);
}

void vn_led_status::Do_led_mqtt_net_err()
{
    vn_led_status::mqtt_interval++;
    switch(vn_led_status::mqtt_interval)
    {
        case 0:
        default:
            ledoff(&status_io.pin_mqtt);
            break;
        case 1:
        case 2:
        case 10:
        case 11:
            ledon(&status_io.pin_mqtt);
            break;
    }
    if(vn_led_status::mqtt_interval<30)    // led hb toggle every 30 tick
        return;
    vn_led_status::mqtt_interval=0;
}

void vn_led_status::set_led_mqtt_net_err()
{
    vn_led_status::links=3;
}

void vn_led_status::set_led_mqtt_normal()
{
    vn_led_status::links=1;
}

void vn_led_status::set_led_mqtt_wait_network()
{
    vn_led_status::links=2;
}


void vn_led_status::Do_led_mqtt_send()
{
    if(vn_led_status::mqtt_send_interval>2)    // led hb off after 2 tick
        {
        ledoff(&status_io.pin_net);
        }

    if(vn_led_status::mqtt_send_interval++<10)    // led hb toggle every 10 tick
        return;
    vn_led_status::mqtt_send_interval=0;
    vn_led_status::mqtt_done=0;
    ledtoggle(&status_io.pin_net);
}

void vn_led_status::set_led_mqtt_send()
{
    vn_led_status::mqtt_done=1;
}


void vn_led_status::performAction()
{
    switch(vn_led_status::status)
    {
        default:
        case 0: //  re-start
            ledoff(&status_io.pin_hb);
            break;
        case 1: // normal HB 
            Do_led_hb_normal();
            break;
        case 2: // node wait WIFI SSID
            Do_led_hb_wait_SSID();
            break;
        case 3: // WIFI SSID error 
            Do_led_hd_net_err();
            break;
    }
    switch(vn_led_status::mems_done)
    {
        default:
        case 0:
            ledoff(&status_io.pin_link);
            break;
        case 1:
            Do_led_mems_done();
            break;
    }
    switch(vn_led_status::links)
    {
        case 0: 
            ledoff(&status_io.pin_mqtt);
            break;
        case 1: //  mqtt connection success
            Do_led_mqtt_normal();
            break;
        case 2: // wait mqtt connection
            Do_led_mqtt_wait_connection();
            break;
        case 3: //  mqtt err connection
            Do_led_mqtt_net_err();
            break;
    }
    switch(vn_led_status::mqtt_done)
    {
        default:
        case 0:
            ledoff(&status_io.pin_net);
            break;
        case 1:
            Do_led_mqtt_send();
            break;
    }
}
