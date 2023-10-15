/*
 * MUST-PES ABU ROBOCON 2018
 * Program by: Shijir
 * 
 * Master Mega
 * 
*/
#include <LCD5110_Basic.h>
#include <PS2X_lib.h>
#include <math.h>
#include <Wire.h>
/*------------------------Input Output Pins------------------------*/
int digitalIO[8] = {23,25,27,29,31,33,35,37};
int mosfet[8] = {39,41,43,45,47,49,51,53};
int button[4] = {48,46,44,42};
int led[4] = {34,36,38,40};
int analogpin[16] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
/*----------DIGITAL IO----*/
#define FRONTSW 0
#define LEFTSW 1
#define BACKSW 2
#define CORNORSENS 3 
#define BALLSENS 4
#define BACKSENS 5
/*-------------MOSFET----*/
#define SHOOT 0
#define LIFT 2
#define CATCH 2
#define LED 3
/*--------ANALOG------*/
#define FRONTSHARP 0
#define LEFTSHARP 1
/*--------------------------SHARP----------------------------*/
/*---------------------------LIDAR-------------------------*/
#include <LIDARLite.h>
LIDARLite lidar;
/*------------------------Serial Variable------------------------*/
byte serAngle,serZ;
double angle,Z;
double fangle;
double x,y;
double oldx,oldy;
byte serr;
byte maxspeed=200;

/*-----------------------------PS2----------------------------*/
PS2X ps2x;
int error=0;
byte vibrate=0;
/*------------------------LCD Variable------------------------*/
LCD5110 LCD(9, 10, 11, 13, 12);
//      CLK  - Pin 9
//      Din  - Pin 10
//      DC   - Pin 11
//      RST  - Pin 3
//      CE   - Pin 12
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
int lid1en = 23;
int lid2en = 25;
/*------------------------Sound------------------------*/
int i=0;
int buzz=8;
int beeph = 100;
/*------------------------Motor------------------------*/
int motorrpm=333;
int s=0;

void setup() {
  
  //for(int i=0;i<8;i++){pinMode(digitalIO[i],INPUT_PULLUP);} 
  //for(int i=0;i<8;i++){pinMode(mosfet[i],OUTPUT);} 
  for(int i=0;i<16;i++){pinMode(analogpin[i],INPUT);} 
  pinMode(buzz,OUTPUT);
  for(int i=0;i<4;i++){pinMode(button[i],INPUT_PULLUP);} 
  for(int i=0;i<4;i++){pinMode(led[i],OUTPUT);}
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  pinMode(lid1en,OUTPUT);
  pinMode(lid2en,OUTPUT);
  //PS2();
  LCD.InitLCD();
  LCD.setFont(SmallFont);
  //error = ps2x.config_gamepad(9, 11, 10, 12, true, true); //(clock-9, command-11, attention-10, data-12)
  //lidar.begin(0,true);
  //lidar.configure(0);
  Wire.begin();
  
  mos(SHOOT,HIGH);
  start_sound();
}



uint64_t inter=1000;
void loop() {
//  tell_uno(1,2,3);
//  delay(1000);
//  tell_uno(4,5,6);
//  delay(1000);
//  tell_uno(7,8,9);
//  delay(1000);
//  tell_uno(10,11,12);
//  delay(1000);
  useshoot(LOW);
  uint32_t t=millis();
  if(t < inter && t > 0){
  mot_go(90,255,127);
  digitalWrite(led[0],HIGH);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],LOW);
  }
  else if( t < (inter+1000) && t > inter){
  mot_go(180,255,127);
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],HIGH);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],LOW);
  }
  else if( t < (inter+2000) && t > (inter+1000)){
  mot_go(-90,255,127);
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],HIGH);
  digitalWrite(led[3],LOW);
  }
  else if( t < (inter+3000) && t > (inter+2000)){
  mot_go(0,255,127);
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],HIGH);
  }
  else{inter+=4000;}
  
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
void useshoot(bool st){
  if(st == HIGH){
    digitalWrite(lid1en,LOW);
    digitalWrite(lid2en,HIGH);
  }
  else{
    digitalWrite(lid1en,HIGH);
    digitalWrite(lid2en,LOW);
  }
}
void mos(int x, bool y){
  digitalWrite(mosfet[x],y);
}
void mot_go(float x,float y,float r){
  disp();
  serZ = y;
  s=s+10;
  fangle=x+180;
  serAngle=fangle*255/360;
  //serZ=y;
  serr=r;
  if(serr > 255){serr=255;}
  if(serr <0){serr=0;}
  if(serZ > 255){serZ=255;}
  tell_uno(serAngle,serZ,serr);
}
/*/------------------------LCD------------------------------*/
void disp(){
  LCD.clrScr();
  LCD.setFont   (SmallFont);
  LCD.print     ("dist: ",LEFT,   0);
  //LCD.print     ("Speed: ",LEFT,   24);
  LCD.setFont   (MediumNumbers);
  LCD.printNumI (lidar.distance(false),   CENTER, 24);
}
/*----------------------Serial Send-------------------------*/
void tell_uno(byte x, byte y, byte z) {
  Wire.beginTransmission(8);
  Wire.write(x);
  Wire.write(y);
  Wire.write(z);
  Wire.write(maxspeed);
  Wire.endTransmission();
  Serial.print(x);
  Serial.print("\t");
  Serial.print(y);
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");
  Serial.println(maxspeed);
}
/*----------------------Sound-------------------------*/
void start_sound(){
  for(i=0;i<1000;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(50);
  }
  delay(20);
  for(i=0;i<1000;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(40);
  }
  delay(20);
  for(i=0;i<1000;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(30);
  }
  delay(20);
  for(i=0;i<1000;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(20);
  }
  delay(20);
  for(i=0;i<1500;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(10);
  }
  delay(1000);
  for(i=0;i<1500;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(30);
  }
}
