#ifndef VN_LED
#define VN_LED

#include <Arduino.h>


typedef struct{
    uint8_t pin;
    uint8_t status;
}type_pin_status;

typedef struct{
    type_pin_status pin_hb;
    type_pin_status pin_net;
    type_pin_status pin_mqtt;
    type_pin_status pin_link;
}type_vn_pin;

class vn_led_status {
public:
    vn_led_status(uint8_t hb,uint8_t net,uint8_t mqtt,uint8_t link,unsigned long tkinterval);  // Constructor to set the pin
    void begin();          // Initialize the pin
    void ledtoggle(type_pin_status *io_pin);         // Toggle the LED state
    void ledon(type_pin_status *io_pin);             // Turn the LED on
    void ledoff(type_pin_status *io_pin);            // Turn the LED off
    void handleTaskStatus();
    void performAction();
    // func set led status
    void set_led_status_wait_network();
    void set_led_status_normal();
    void set_led_status_net_err();
    // func set led mems
    void set_led_mems_done();
    // func set led mqtt network
    void set_led_mqtt_wait_network();
    void set_led_mqtt_normal();
    void set_led_mqtt_net_err();
    //  func set led mqtt send
    void set_led_mqtt_send();

    void handelTaskNetWork();
    void performNetWork();

    type_vn_pin status_io;
    
private:
    uint8_t status;
    uint8_t links;
    uint8_t mems_done;
    uint8_t mqtt_done;
    uint8_t _pin;
    uint16_t hb_interval;
    uint16_t mems_interval;
    uint16_t mqtt_send_interval;
    uint16_t mqtt_interval;

    unsigned long _lastMillis;      // Tracks time for periodic tasks
    unsigned long _interval;        // Interval for background task

    void Do_led_hb_normal();
    void Do_led_hb_wait_SSID();
    void Do_led_hd_net_err();

    void Do_led_mems_done();

    void Do_led_mqtt_normal();
    void Do_led_mqtt_wait_connection();
    void Do_led_mqtt_net_err();

    void Do_led_mqtt_send();
    
};

#endif // BLINKER_H
