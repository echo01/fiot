#include <Arduino.h>
#include <vibration_node.h>
#include <vn_web_module.h>
#include <vn_web_ov.h>
// put function declarations here:

vn_led_status op_vnled(25,26,33,32,100);
vn_module vn_m(1000,80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("=====\t\t TP-VN-01 \t\t=====");
  Serial.println("WIFI NODE VIBRATION NODE SENSOR ");
  op_vnled.status_io.pin_hb.pin=25;
  op_vnled.status_io.pin_link.pin=26;
  op_vnled.status_io.pin_mqtt.pin=33;
  op_vnled.status_io.pin_net.pin=32;
  vn_m.begin();
  // pinMode(op_vn.status_io.pin_hb.pin, OUTPUT);
  // digitalWrite(op_vn.status_io.pin_hb.pin, HIGH);

  // pinMode(op_vn.status_io.pin_link.pin, OUTPUT);
  // digitalWrite(op_vn.status_io.pin_link.pin, HIGH);

  // pinMode(op_vn.status_io.pin_mqtt.pin, OUTPUT);
  // digitalWrite(op_vn.status_io.pin_mqtt.pin, HIGH);
  op_vnled.begin();

  // op_vnled.set_led_status_net_err();
  // op_vnled.set_led_mqtt_wait_network();
  // op_vnled.set_led_mems_done();
  // op_vnled.set_led_mqtt_send();

}

void loop() {
  // put your main code here, to run repeatedly:
  op_vnled.handleTaskStatus();
  switch (vn_m.net.status)
    {
    case 255:   //reconnect
    case VN_NET_START: // Start WIFI
      op_vnled.ledon(&op_vnled.status_io.pin_link);
      break;
    case VN_NET_WAIT_CONNECT:
      op_vnled.set_led_status_wait_network();
      break;
    case VN_NET_STARTED:
      if( (vn_m.net.mode==VN_NET_MODE_STA)&&(vn_m.net_status==VN_NET_STA) )
            {
            op_vnled.set_led_status_normal();
            }
      else
            {
            op_vnled.ledon(&op_vnled.status_io.pin_link);
            }
      break;
    case VN_NET_MQTT_START:
      break;
    }

}

