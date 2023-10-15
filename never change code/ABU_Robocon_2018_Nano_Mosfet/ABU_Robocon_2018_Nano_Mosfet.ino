/*
 * MUST-PES ABU ROBOCON 2018
 * Program by: Shijir
 * 
 * Slave Nano Mosfet
 * 
*/
#include<Wire.h>
byte data;
int data0,data1,data2,data3,data4,data5,data6,data7;
void setup() {
  Serial.begin(9600);
  Wire.begin(10); //0x0b;
  Wire.onReceive(receiveEvent);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(5,OUTPUT);
}

void loop() { 
  datasep();
  if(data0 == 1){digitalWrite(12,HIGH);}
  else{digitalWrite(12,LOW);}
  if(data1 == 1){digitalWrite(11,HIGH);}
  else{digitalWrite(11,LOW);}
  if(data2 == 1){digitalWrite(10,HIGH);}
  else{digitalWrite(10,LOW);}
  if(data3 == 1){digitalWrite(9,HIGH);}
  else{digitalWrite(9,LOW);}
  if(data4 == 1){digitalWrite(8,HIGH);}
  else{digitalWrite(8,LOW);}
  if(data5 == 1){digitalWrite(7,HIGH);}
  else{digitalWrite(7,LOW);}
  if(data6 == 1){digitalWrite(6,HIGH);}
  else{digitalWrite(6,LOW);}
  if(data7 == 1){digitalWrite(5,HIGH);}
  else{digitalWrite(5,LOW);}
}
void datasep(){
  data0 = data >> 7;
  data1 = data >> 6 & B01;
  data2 = data >> 5 & B001;
  data3 = data >> 4 & B0001;
  data4 = data >> 3 & B00001;
  data5 = data >> 2 & B000001;
  data6 = data >> 1 & B0000001;
  data7 = data & B00000001; 
}
void receiveEvent(int howMany) {
  while (1 < Wire.available());
  data = Wire.read();
  Serial.println(data);
}
