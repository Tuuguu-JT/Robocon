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
#include <Servo.h>
/*------------------------Input Output Pins------------------------*/
int digitalIO[8] = {23,25,27,29,31,33,35,37};
int mosfet[8] = {39,41,43,45,47,49,51,53};
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
byte mosdata = B00000000;
/*--------ANALOG------*/
#define FRONTSHARP 0
#define LEFTSHARP 1
/*--------------------------SHARP----------------------------*/
/*---------------------------LIDAR-------------------------*/
#include<Wire.h>
#include<LIDARLite.h>
LIDARLite lidar;
Servo lidservo;
/*------------------------Serial Variable------------------------*/
byte serAngle,serZ;
double angle,Z;
double fangle;
double x,y;
double oldx,oldy;
byte serr;
byte maxspeed=100;

/*------------------------LCD Variable------------------------*/
LCD5110 LCD(7, 6, 5, 3, 4);
//      CLK  - Pin 7
//      Din  - Pin 6
//      DC   - Pin 5
//      RST  - Pin 3
//      CE   - Pin 4
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];

/*------------------------Sound------------------------*/
int i=0;
int buzz=8;
int beeph = 100;
/*------------------------Motor------------------------*/
int motorrpm=333;


void setup() {
  
  for(int i=0;i<8;i++){pinMode(digitalIO[i],INPUT_PULLUP);} 
  for(int i=0;i<8;i++){pinMode(mosfet[i],OUTPUT);} 
  for(int i=0;i<16;i++){pinMode(analogpin[i],INPUT);} 
  pinMode(buzz,OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  
  //PS2();
  LCD.InitLCD();
  LCD.setFont(SmallFont);
  //error = ps2x.config_gamepad(9, 11, 10, 12, true, true); //(clock-9, command-11, attention-10, data-12)
  lidservo.attach(37);
  Wire.begin();
  mos(SHOOT,HIGH);
  start_sound();

}

void loop() {
}
/*
 * Function1
 */
void func1(){while(1){
  lidservo.writeMicroseconds(1540);
  while(1){
    //disp();
    mot_go(0,255,0);
    int dist=lidar.distance(false);
    if(dist<370 && dist>360){break;}
  }
  mot_go(0,0,0);
  lidservo.writeMicroseconds(570);
  delay(100);
  while(1){
    mot_go(90,255,0);
    int dist=lidar.distance(false);
    if(dist<160 && dist>140){break;}
  }
  while(1){
    mot_go(180,50,0);
    if(digitalRead(digitalIO[LEFTSW])==LOW){delay(300);break;}
  }
  while(1){
    //disp();
    lidservo.write(570);
    mot_go(-90,50,0);
    int dist=lidar.distance(false);
    if(dist<155 && dist>145){break;}
  }
  cornordetect();
  while(1){
    mot_go(0,0,0);
    if(digitalRead(digitalIO[BALLSENS]) == LOW){break;}
  }
  break;
}}
/*
 * Function 2
 */
void func2(){while(1){
  lidservo.write(570);
  while(1){
    mot_go(0,0,0);
    if(digitalRead(digitalIO[BACKSENS]) == HIGH){delay(1000);break;}
  }
  mos(CATCH,HIGH);
  delay(100);
  useshoot(true);
  while(1){
    mot_go(45,100,-100);
    disp();
    int dist=lidar.distance(false);
    if(dist<367 && dist>307){break;}
  }
  Shoot(HIGH);
  useshoot(false);
  while(1){
    mot_go(-45,100,100);
    int dist=lidar.distance(false);
    if(digitalRead(digitalIO[LEFTSW] == LOW)){break;}
  }
  cornordetect();
  while(1){
    mot_go(0,0,0);
    if(digitalRead(digitalIO[BALLSENS]) == LOW){break;}
  }
  break;
}}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/

/*
 * Shoot(change lidar)---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void useshoot(bool x){
  
}
/*
 * Cornor detect algorithm---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void cornordetect(){
  while(1){
    if(digitalRead(digitalIO[CORNORSENS]) == HIGH){
      while(1){
        mot_go(90,50,0);
        if(digitalRead(digitalIO[CORNORSENS]) == LOW){break;}
      }
    }
    else{
      while(1){
        mot_go(-90,50,0);
        if(digitalRead(digitalIO[CORNORSENS]) == HIGH){break;}
      }
      while(1){
        mot_go(135,50,0);
        if(digitalRead(digitalIO[CORNORSENS]) == LOW){break;}
      }
    }
    break;
  }
}
/*
 * Shoot algorithm---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void Shoot(bool st){
  mot_go(0,0,0);
  delay(100);
  mot_go(0,0,0);
  mos(CATCH,LOW);
  mos(LED,HIGH);
  delay(100);
  mot_go(0,0,0);
  if(st==HIGH){mos(LIFT,HIGH);}
  else{mos(LIFT,LOW);}
  mos(LED,LOW);
  delay(100);
  mot_go(0,0,0);
  mos(LED,HIGH);
  delay(100);
  mot_go(0,0,0);
  mos(LED,LOW);
  delay(100);
  mot_go(0,0,0);
  mos(LED,HIGH);
  delay(100);
  mot_go(0,0,0);
  mos(LED,LOW);
  delay(100);
  mot_go(0,0,0);
  mos(LED,HIGH);
  delay(100);
  mos(LED,LOW);
  delay(500);
  mos(SHOOT,HIGH);
  delay(1000);
  mos(LED,LOW);
  mos(SHOOT,LOW);
  if(st==HIGH){mos(LIFT,LOW);}
  else{mos(LIFT,LOW);}
}
/*
 * Mosfet sending data---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void mos(int x, bool y){
  byte highdata = B10000000 >> x;;
  byte lowdata = ~highdata;
  if(y == HIGH){
    mosdata |= highdata;
  }
  else{
    mosdata &= lowdata;
  }
  Wire.beginTransmission(10);
  Wire.write(mosdata);
  Wire.endTransmission();
}
/*
 * Motor data calculation---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void mot_go(float x,float y,float r){
  fangle=x+180;
  serAngle=fangle*255/360;
  serZ=y;
  serr=r/2+127;
  if(serr > 255){serr=255;}
  if(serr <0){serr=0;}
  
  tell_uno(serAngle,serZ,serr);
}
/*
 * Display ---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void disp(){
  LCD.clrScr();
  LCD.setFont   (SmallFont);
  LCD.print     ("dist: ",LEFT,   0);
  //LCD.print     ("Speed: ",LEFT,   24);
  LCD.setFont   (MediumNumbers);
  //LCD.printNumI (Z,   CENTER, 24);
}
/*
 * Motor sending data ---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
void tell_uno(byte x, byte y, byte z) {
  Wire.beginTransmission(8);
  Wire.write(x);
  Wire.write(y);
  Wire.write(z);
  Wire.write(maxspeed);
  Wire.endTransmission();
//  Serial.print(x);
//  Serial.print("\t");
//  Serial.print(y);
//  Serial.print("\t");
//  Serial.print(z);
//  Serial.print("\t");
//  Serial.println(maxspeed);
}
/*
 * Sound---------------------------------------------------------------------------------------------------------------------------------------------------------
 */
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
