#include <Arduino.h>
#include "master_mb.h"
#include "node_op.h"

// put function declarations here:
master_mb   mb1(&Serial1,1000,100,50,250);
NodeNetwork km07_node;

void setup() {
  Serial.begin(115200);
  Serial.println("Initial Application");
  // Serial1.begin(115200,SERIAL_8N1,5,17);

  // put your setup code here, to run once:
 // mb1.ConfigSerial(1,1,0,3);    //  8 bits,1 stop, none,19200
  mb1.serialPtr->begin(115200,SERIAL_8N1,5,17,false,500);
  // mb1.ConfigSerial(d8bits,stopbits1,parity_none,bps9600);
  // Serial1.println("Initial Application");

  km07_node.setSerial(&Serial, true);  // Enable Serial output
  km07_node.setSerial1(&Serial1,false);
  km07_node.runTask(); // Start Ethernet (No WiFi)

}

void loop() {
  // put your main code here, to run repeatedly:
  // mb1.handelTask();
  // km07_node.handleTelnetClients_ex();
}

