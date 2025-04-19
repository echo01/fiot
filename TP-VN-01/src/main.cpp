#include <Arduino.h>
#include <main.h>
// #include <vibration_node.h>
// #include <vn_web_module.h>
// #include <vn_web_ov.h>
// #include "SPIFFS.h"
// put function declarations here:
// File file;
// File root;
vn_led_status op_vnled(25,26,33,32,100);
vn_module vn_m(1000,80);
vn_WebPage vn_Web(1000);
mems vn_mems(1);
int esp_stage;
unsigned long op_tick;
unsigned long time_to_sleep;

void IRAM_ATTR handleMotionInterrupt() {
  // Just wake ESP32, nothing to do here in ISR
  vn_mems.motionDetected=true;
  // Serial.print("MEMS sensor hit thresholdvalue:");
  // Serial.printf("%f mg\tx:%0.3f,y:%0.3f,z:%0.3f\r\n",vn_mems.mems_THRESH_ACT*62.5,vn_mems.mems_data.x.RmsG,vn_mems.mems_data.y.RmsG,vn_mems.mems_data.z.RmsG);
}

void IRAM_ATTR handleMotionInterrupt14() {
  vn_mems.motionDetected=true;
  // Serial.print("MEMS sensor hit thresholdvalue:14");
}


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
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  vn_Web.listDir();

  // pinMode(op_vn.status_io.pin_hb.pin, OUTPUT);
  // digitalWrite(op_vn.status_io.pin_hb.pin, HIGH);

  // pinMode(op_vn.status_io.pin_link.pin, OUTPUT);
  // digitalWrite(op_vn.status_io.pin_link.pin, HIGH);

  // pinMode(op_vn.status_io.pin_mqtt.pin, OUTPUT);
  // digitalWrite(op_vn.status_io.pin_mqtt.pin, HIGH);
  op_vnled.begin();
  Serial.println("................");
  Serial.println("ADXL_345 SENSOR");
  delay(1000);
  Wire.begin();
  Wire.setClock(400000); // Set I2C clock speed to 400 kHz
  vn_mems.set_mems_data(&mems_data);
  vn_mems.init_adxl3245_module();
  pinMode(ADXL_INT1, INPUT_PULLUP);
  pinMode(ADXL_INT2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ADXL_INT1), handleMotionInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(ADXL_INT2), handleMotionInterrupt14, RISING);

  // Task_Adxl345.attach_ms(st01.mems_task_period,&vn_mems.Task_adxl345_mems);
  // op_vnled.set_led_status_net_err();
  // op_vnled.set_led_mqtt_wait_network();
  // op_vnled.set_led_mems_done();
  // op_vnled.set_led_mqtt_send();

}


void operate_task()
{
  unsigned long currentMills=millis();
  if(currentMills-op_tick< 1000)
  {
    return;
  }
  op_tick = currentMills;

}

void loop() {
  // put your main code here, to run repeatedly:
  op_vnled.handleTaskStatus();
  // vn_mems.Task_read_mems();
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

