/*
 * MUST-PES ABU ROBOCON 2018
 * Program by: Shijir
 * 
 * Slave Nano #1
 * 
*/

#include <Wire.h>
#include <LIDARLite.h>
LIDARLite Lidar;

byte highbyte=0x00;
byte lowbyte=0x00;
byte startbyte=0x9A;
byte paritybyte=0;
int paritybit=0;
void setup() {
  Serial.begin(9600);
  while(!Serial);
  Lidar.begin(0, true,0xC4);
  Lidar.configure(0);
}

/*----------------------------Main Loop------------------------------*/
void loop() {
  uint16_t dist = Lidar.distance();
  Serial.println(dist);
}
