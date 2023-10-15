/*
 * MUST-PES ABU ROBOCON 2018
 * Program by: Shijir
 * 
 * Master Mega
 * 
*/
#include <LCD5110_Basic.h>
#include <math.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include<LIDARLite.h>
/*------------------------Input Output Pins------------------------*/
int digitalIO[11] = {27,29,31,33,35,37,39,41,43,45,47};
int analogpin[8] = {A0,A1,A2,A3,A4,A5,A6,A7};
int button[4] = {48,46,44,42};
int led[4] = {34,36,38,40};
/*------------------------SD------------------------*/
const int chipSelect = 7;
int SDdata=0;
String sddaat[4];
String SDstring = "";
File dataFile;
int sddel=12;
/*----------DIGITAL IO----*/
#define FRONTSW1 0   //Front Right
#define FRONTSW2 1   // Front Left
#define LEFTSW1 2    // Left Front
#define LEFTSW2 3    // Left Back
#define CORNERSENS 4 
#define BALLSENS 5
#define BACKSENS1 6
#define BACKSENS2 7
/*-------------SHOOT----*/
#define FORWARD 1
#define BACKWARD 2
#define STOP 3
/*-------------MOSFET----*/
#define SHOOT 6
#define LIFT 2
#define CATCH 3
#define LED 4
#define PR1 5   // Normal shuttlecock throwing pressure
#define PR2 7   // Golden shuttlecock throwing pressure
byte mosdata = B00000000;
/*--------ANALOG------*/
#define LEFT1SHARP 0
#define LEFT2SHARP 1
/*--------------------------SHARP----------------------------*/
#include <SharpIR.h>
SharpIR left1(GP2Y0A02YK0F,analogpin[LEFT1SHARP]);
SharpIR left2(GP2Y0A02YK0F,analogpin[LEFT2SHARP]);
int ninetydiff = 6;
/*---------------------------LIDAR-------------------------*/
LIDARLite lidar;
Servo lidservo;
int lid1en = 23;
int lid2en = 25;
//Servo 0    510
//Servo 90   140
//Servo 180  2480
int tz1x = 300;
int tz1y = 140;
int tz1ycorner = 150;
int tree1 = 336;
int tree2 = 520;
int tree3 = 510;
int tz3x = 240; 
int tz3y = 580;
int treespd=55;
int backspd=65;
/*------------------------Serial Variable------------------------*/
byte serAngle,serZ;
double angle,Z;
double fangle;
double x,y;
double oldx,oldy;
double r;
byte serr;
byte maxspeed=100;
byte maxspeednorm=150;
byte ch[11]={0,0,0,0,0,0,0,0,0,0,0};

/*------------------------LCD Variable------------------------*/
LCD5110 LCD(9, 10, 11, 13, 12);
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
int s = 0;

void setup() {
  
  for(int i=0;i<13;i++){pinMode(digitalIO[i],INPUT);} 
  for(int i=0;i<4;i++){pinMode(button[i],INPUT_PULLUP);} 
  for(int i=0;i<4;i++){pinMode(led[i],OUTPUT);} 
  for(int i=0;i<8;i++){pinMode(analogpin[i],INPUT);} 
  pinMode(buzz,OUTPUT);
  //pinMode(lidservo,OUTPUT);
  pinMode(lid1en,OUTPUT);
  pinMode(lid2en,OUTPUT);
  Serial.begin(9600);
  
  LCD.InitLCD();
  LCD.setFont(SmallFont);
  digitalWrite(lid1en,HIGH);
  digitalWrite(lid2en,LOW);
  Wire.begin();
//  Serial.print("Initializing SD card...");
//  if (!SD.begin(chipSelect)) {
//    Serial.println("Card failed, or not present");
//    return;
//  }
//  Serial.println("card initialized.");
//  SD.remove("move.txt");
//  
  mos(SHOOT,LOW);
  start_sound();

}

void loop() {
  //Radiolink();
  //mot_go(angle,Z,r);
  if(digitalRead(button[0]) == LOW){shoot_rot(FORWARD,treespd);}
  if(digitalRead(button[1]) == LOW){shoot_rot(BACKWARD,backspd);}
  if(digitalRead(button[2]) == LOW){shoot_rot(FORWARD,backspd);}
  if(digitalRead(button[3]) == LOW){shoot_rot(STOP,treespd);}
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],LOW);
}
/*--------------------------------------------------------------------------FUNCTION----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------Mini Function-------------------------------------------------------------------------*/
/*------------------Lidar Break---------------*/
bool lidgo(int range){
  int dist = lidar.distance(false);
  disp(dist);
  if(dist < (range+20) && dist > (range-20)){
    return true;
  }
  else{return false;}
}
/*------------------Sharp Break---------------*/
bool sharpgo(int diff){
  int shdist1 = left1.getDistance();
  int shdist2 = left2.getDistance();
  int shdiff = abs(shdist1-shdist2);
  if(shdiff < diff){return true;}
  else {return false;}
}
/*-----------------Motor Calculation-------------------------*/
void mot_go(float x,float y,float r){
  if(s < y){
    serZ=s;
  }
  else{
    serZ = y;
  }
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
void mot_stop(){
  tell_uno(127,0,127);
  s=0;
  delay(50);
}
void shoot_rot(int xdir,int xspd){
  Wire.beginTransmission(11);
  Wire.write(xdir);
  Wire.write(xspd);
  Wire.endTransmission();
}
/*---------------------Mosfet-----------------*/
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
/*---------------------SD card write------------------------*/
void sdwrite(String a,String b, String c,String d){
  String movestr = a; movestr += ",";
  movestr += b; movestr += ",";
  movestr += c; movestr += ",";
  movestr += d; movestr += ",";movestr += "\n";
  dataFile = SD.open("move.txt", FILE_WRITE);
  dataFile.println(movestr);
  dataFile.close();
}
/*---------------------SD card read------------------------*/
void sdread(){
  dataFile = SD.open("move.txt");
  while(dataFile.available()){
  SDdata = dataFile.read(); SDstring += (char)SDdata;
  if(SDdata == '\n'){
//    Serial.println(SDstring);
    int seprator[5] = {0};int n=0,startfrom=0;
    for(n=1;n<=4;n++){
      seprator[n] = SDstring.indexOf(',',startfrom);startfrom = seprator[n]+1;
      seprator[0] = -1;
    }
    for(int count = 0; count < 4;count++){
      String sdrawdata = "";
      for(int index = (seprator[count]+1);index < seprator[count+1];index++){
        sdrawdata = SDstring.charAt(index); sddaat[count] += sdrawdata;
      }
    }
    angle = sddaat[0].toInt();
    Z = sddaat[1].toInt();
    r=sddaat[2].toInt();
    maxspeed = sddaat[3].toInt();
    if(maxspeed != 0){
    mot_go(angle,Z,r);
    delay(10);
//    Serial.print(angle);
//    Serial.print("  ");
//    Serial.print(Z);
//    Serial.print("  ");
//    Serial.print(r);
//    Serial.print("  ");
//    Serial.println(maxspeed);
    }
    sddaat[0] = "";sddaat[1] = "";sddaat[2] = "";sddaat[3] = "";
    SDstring = "";
  }
  }
  
  dataFile.close();
}
/*-----------------------Shoot-------------------------------*/
void Shoot(bool st){while(1){
  if(st==HIGH){
    mos(PR1,HIGH);
    mos(PR2,LOW);
  }
  else{
    mos(PR1,LOW);
    mos(PR2,HIGH);
  }
  mot_go(0,0,127);
  delay(100);
  mot_go(0,0,127);
  mos(CATCH,LOW);
  mos(LED,HIGH);
  delay(100);
  mot_go(0,0,127);
  if(st==HIGH){mos(LIFT,HIGH);}
  else{mos(LIFT,LOW);}
  mos(LED,LOW);
  delay(100);
  mot_go(0,0,127);
  mos(LED,HIGH);
  delay(100);
  mot_go(0,0,127);
  mos(LED,LOW);
  delay(100);
  mot_go(0,0,127);
  mos(LED,HIGH);
  delay(100);
  mot_go(0,0,127);
  mos(LED,LOW);
  delay(100);
  mot_go(0,0,127);
  mos(LED,HIGH);
  delay(100);
  mos(LED,LOW);
  if(st==HIGH){delay(500);}
  mos(SHOOT,HIGH);
  delay(1000);
  mos(LED,LOW);
  mos(SHOOT,LOW);
  if(st==HIGH){mos(LIFT,LOW);}
  else{mos(LIFT,LOW);}
  break;
}
}
/*----------------Useshoot---------------------------------*/
void useshoot(bool st){
  if(st == HIGH){
    digitalWrite(lid1en,LOW);
    digitalWrite(lid2en,HIGH);
  }
  else{
    digitalWrite(lid1en,HIGH);
    digitalWrite(lid2en,LOW);
  }
  delay(50);
}
/*-----------------Lidar servo--------------------------*/
void lidserv(int a){
  switch(a){
    case 0:
    lidservo.writeMicroseconds(510);
    break;
    case 90:
    lidservo.writeMicroseconds(1450);
    break;
    case 180:
    lidservo.writeMicroseconds(2380);
    break;
    default:
    break;
  }
  delay(100);
}
/*-----------------Corner detect------------------------*/
void cornerdetect(){
  maxspeed=200;
  s=256;
  while(1){
    if(digitalRead(digitalIO[CORNERSENS]) == HIGH){
      while(1){
        mot_go(90,100,127);
        if(digitalRead(digitalIO[CORNERSENS]) == LOW){break;}
      }
      while(1){
        mot_go(180,50,127);
        if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){break;}
      }
      while(1){
        mot_go(-95,100,127);
        if(digitalRead(digitalIO[CORNERSENS]) == HIGH){break;}
      }
      break;
    }
    else{
      while(1){
        mot_go(180,50,127);
        if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){break;}
      }
      while(1){
        mot_go(-95,100,127);
        if(digitalRead(digitalIO[CORNERSENS]) == HIGH){break;}
      }
      break;
    }
  }
  maxspeed=maxspeednorm;
  s=0;
}
/*----------------Parallel---------------------------------*/
void parallelleft(){while(1){
  if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == HIGH){
    while(1){
      maxspeed = 40;
      mot_go(-135,255,255);
      if(digitalRead(digitalIO[LEFTSW2]) == LOW){s=0;break;} 
    }
    maxspeed = maxspeednorm;
    break;
  }
  else if(digitalRead(digitalIO[LEFTSW1]) == HIGH && digitalRead(digitalIO[LEFTSW2]) == LOW){
    while(1){
      maxspeed = 40;
      mot_go(135,255,0);
      if(digitalRead(digitalIO[LEFTSW1]) == LOW){s=0;break;}
    }
    maxspeed = maxspeednorm;
    break;
  }
  else if(digitalRead(digitalIO[LEFTSW1]) == HIGH && digitalRead(digitalIO[LEFTSW2]) == HIGH){
    while(1){
      maxspeed=maxspeednorm;
      mot_go(180,50,127);
      if(digitalRead(digitalIO[LEFTSW1]) == LOW || digitalRead(digitalIO[LEFTSW2]) == LOW){s=0;break;}
    }
  }
  else if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){s=0;break;}
}
}
void parallelfront(){while(1){
  if(digitalRead(digitalIO[FRONTSW1]) == LOW && digitalRead(digitalIO[FRONTSW2]) == HIGH){
    while(1){
      maxspeed = 40;
      mot_go(135,255,255);
      if(digitalRead(digitalIO[FRONTSW2]) == LOW){s=0;break;} 
    }
    maxspeed = maxspeednorm;
    break;
  }
  else if(digitalRead(digitalIO[FRONTSW1]) == HIGH && digitalRead(digitalIO[FRONTSW2]) == LOW){
    while(1){
      maxspeed = 40;
      mot_go(45,255,0);
      if(digitalRead(digitalIO[FRONTSW1]) == LOW ){s=0;break;}
    }
    maxspeed = maxspeednorm;
    break;
  }
  else if(digitalRead(digitalIO[FRONTSW1]) == HIGH && digitalRead(digitalIO[FRONTSW2]) == HIGH){
    while(1){
      maxspeed=maxspeednorm;
      mot_go(90,50,127);
      if(digitalRead(digitalIO[FRONTSW1]) == LOW || digitalRead(digitalIO[FRONTSW2]) == LOW){s=0;break;}
    }
  }
  else if(digitalRead(digitalIO[FRONTSW1]) == LOW && digitalRead(digitalIO[FRONTSW2]) == LOW){s=0;break;}
}
}

/*/------------------------LCD------------------------------*/
void disp(int dispdist){
  LCD.clrScr();
  LCD.setFont   (SmallFont);
  LCD.print     ("dist: ",LEFT,   0);
  LCD.setFont   (MediumNumbers);
  LCD.printNumI (dispdist,   CENTER, 24);
}
/*----------------------Serial Receive-------------------------*/
void Radiolink() {
  Wire.requestFrom(9, 11);
  while (Wire.available()) { 
    for(int i=0;i<11;i++){
      ch[i] = Wire.read();
      if(ch[i] < 130 && ch[i]>123){ch[i]=127;}      
    }
  }
  x =(ch[3] - 127)*2;
  y =(ch[1] - 127)*2;
  r=ch[0];
  if(x>-50&&x<50){x=0;}
  if(y>-50&&y<50){y=0;}
  if(x>255){x=255;}
  if(x<-255){x=-255;}
  if(y>255){y=255;}
  if(y<-255){y=-255;} 
  angle=atan2(y,x)*180/PI;
  Z=pow(pow(y,2)+pow(x,2),0.5);
  if(Z>255){Z=255;}
  maxspeed = ch[4];
}
/*----------------------Serial Send-------------------------*/
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
  delay(1);
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
