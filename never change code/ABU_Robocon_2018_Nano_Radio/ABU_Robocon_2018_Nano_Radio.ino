/*
 * MUST-PES ABU ROBOCON 2018
 * Program by: Shijir
 * 
 * Slave Nano Radio
 * 
*/
#include <PinChangeInterrupt.h>
#include <Wire.h>
int ch_pin[11] = {2,3,4,5,6,7,8,9,10,11,12};
volatile unsigned long rising_start[11] = {0,0,0,0,0,0,0,0,0,0,0};
volatile long channel_length[11] = {0,0,0,0,0,0,0,0,0,0,0};
int chpro[11]={0,0,0,0,0,0,0,0,0,0,0};
byte ch[11]={0,0,0,0,0,0,0,0,0,0,0};
void setup() {
  Serial.begin(115200);
  for(int i=0;i<11;i++){
    pinMode(ch_pin[i],INPUT);
  }
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[0]),onRising0,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[1]),onRising1,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[2]),onRising2,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[3]),onRising3,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[4]),onRising4,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[5]),onRising5,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[6]),onRising6,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[7]),onRising7,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[8]),onRising8,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[9]),onRising9,CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ch_pin[10]),onRising10,CHANGE);
  Wire.begin(9);
  Wire.onRequest(requestEvent);
}
void onRising0(void){
  noInterrupts();
  processPin(0);
  interrupts();
}
void onRising1(void){
  noInterrupts();
  processPin(1);
  interrupts();
}
void onRising2(void){
  noInterrupts();
  processPin(2);
  interrupts();
}
void onRising3(void){
  noInterrupts();
  processPin(3);
  interrupts();
}
void onRising4(void){
  noInterrupts();
  processPin(4);
  interrupts();
}
void onRising5(void){
  noInterrupts();
  processPin(5);
  interrupts();
}
void onRising6(void){
  noInterrupts();
  processPin(6);
  interrupts();
}
void onRising7(void){
  noInterrupts();
  processPin(7);
  interrupts();
}
void onRising8(void){
  noInterrupts();
  processPin(8);
  interrupts();
}
void onRising9(void){
  noInterrupts();
  processPin(9);
  interrupts();
}
void onRising10(void){
  noInterrupts();
  processPin(10);
  interrupts();
}
void processPin(byte pin){
  uint8_t trigger=getPinChangeInterruptTrigger(digitalPinToPCINT(ch_pin[pin]));
  if(trigger == RISING){
    rising_start[pin] = micros();
  } else if(trigger == FALLING){
    channel_length[pin] = micros() - rising_start[pin];
  }
}
void loop() {
  for(int i=0;i<11;i++){
    chpro[i] = channel_length[i] - 1500 + 400;
    if(chpro[i] < 0){chpro[i] = 0;}
    if(chpro[i] > 800){chpro[i] = 800;}
    ch[i]=map(chpro[i],0,800,255,0);
  }
  if(ch[0]==255 && ch[1]==255 && ch[2]==255 && ch[3]==255 && ch[4]==255 && ch[5]==255 && ch[6]==255 && ch[7]==255 && ch[8]==255 && ch[9]==255 && ch[10]==255){
      for(int i=0;i<11;i++){ch[i]=127;}
  }
  for(int i=0;i<11;i++){
    Serial.print("CH");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(ch[i]);
    Serial.print("\t");
  }
  Serial.println();
}
void requestEvent(){
  Wire.write(ch,11);
}

