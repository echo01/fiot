#include <Arduino.h>
#include "sensor_board.h"

type_st01 st01={
    .humidity=0,
    // .temperature=0,
    // .tick_ms_airsensor=0,
    // .voc_index=0,
    // .voc_raw=0,
    .read_task=&Task_st01_read
};

type_mqtt_value mqtt_node;

void Task_st01_read()
{
  st01.temperature = sht31.readTemperature();
  st01.humidity = sht31.readHumidity();
  st01.voc_index = sgp.measureVocIndex(st01.temperature, st01.humidity);
  

  if (millis() - st01.tick_ms_airsensor >= 5000) {
    st01.tick_ms_airsensor = millis();
    // Serial.print("Temp *C = "); Serial.print(st01.temperature); Serial.print("\t\t");
    // Serial.print("Hum. % = "); Serial.println(st01.humidity);
    // Serial.print("Voc Index: ");
    // Serial.println(st01.voc_index);
    }
}
