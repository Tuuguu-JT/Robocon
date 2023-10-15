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
  lidar.begin(0,true);
  lidar.configure(0);
  lidservo.attach(49,400,2500);
  
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
  disp(lidar.distance(false));
  delay(100);
  if(digitalRead(button[0]) == LOW){
    func1();
    func2();
    func3();
    while(1){
    func4();
    }
  }
  else{mot_go(0,0,127);}
  if(digitalRead(button[1]) == LOW){func2();}
  else{mot_go(0,0,127);}
  if(digitalRead(button[2]) == LOW){func3();}
  else{mot_go(0,0,127);}
  if(digitalRead(button[3]) == LOW){func4();}
  else{mot_go(0,0,127);}
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],LOW);
}
/*--------------------------------------------------------------------------FUNCTION----------------------------------------------------------------------------*/
void func1(){while(1){
  maxspeed=200;
  digitalWrite(led[0],HIGH);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],LOW);
  arstotz1();
  mot_stop(); 
  while(1){
    mot_go(180,150,127);
    if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){delay(250);s=0;break;}
  }
//  parallelleft();
//  while(1){
//    mot_go(180,150,127);
//    if(digitalRead(digitalIO[LEFTSW1]) == LOW || digitalRead(digitalIO[LEFTSW2]) == LOW){delay(500);s=0;break;}
//  }
  //lidserv(0);
//  while(1){
//    mot_go(-90,150,127);
//    if( lidgo(tz1ycorner) == true ) {s=0;break;}
//  }
  cornerdetect();
  break;
}
}

void func2(){while(1){
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],HIGH);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],LOW);
  while(1){
    mot_stop();
    if(digitalRead(digitalIO[BALLSENS]) == LOW && digitalRead(digitalIO[BACKSENS1]) == HIGH && digitalRead(digitalIO[BACKSENS2]) == HIGH){delay(500);break;}  
  }
  mos(PR1,HIGH);
  mos(CATCH,HIGH);
  useshoot(HIGH);
  maxspeed = 35;
  while(1){
    mot_go(45,255,0);
    if( lidgo(tree1) == true){s=0;break;}
  }
  maxspeed=40;
  Shoot(HIGH);
  s=256;
  mot_go(-135,255,255);
  delay(1300);
  s=0;
//  while(1){
//    mot_go(-135,255,255);
//    if( sharpgo(ninetydiff) == true /*|| digitalRead(digitalRead(digitalIO[LEFTSW1]) == LOW || digitalRead(digitalIO[LEFTSW2]) == LOW)*/){s=0;break;}
//  }
  maxspeed=maxspeednorm;
  useshoot(LOW);
  //parallelleft();
  s=256;
  while(1){
    mot_go(180,100,127);
    if(digitalRead(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW)){delay(250);s=0;break;}
  }
  lidserv(0);
//  while(1){
//    mot_go(-95,150,127);
//    if( lidgo(tz1ycorner) == true || digitalRead(digitalIO[CORNERSENS]) == HIGH){s=0;break;}
//  }
  cornerdetect();
  break;
}
}
void func3(){while(1){
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],HIGH);
  digitalWrite(led[3],LOW);
  while(1){
    mot_stop();
    if(digitalRead(digitalIO[BALLSENS]) == LOW && digitalRead(digitalIO[BACKSENS1]) == HIGH && digitalRead(digitalIO[BACKSENS2]) == HIGH){delay(500);break;}  
  }
  mos(PR1,HIGH);
  mos(CATCH,HIGH);
  s=256;
  maxspeed=190;
  tz2totz3();
  s=0;
  while(1){
    mot_go(180,150,127);
    if(digitalRead(digitalIO[LEFTSW1]) == LOW || digitalRead(digitalIO[LEFTSW2]) == LOW){delay(250);s=0;break;}
  }
  mot_stop();
  delay(100);
  s=256;
  useshoot(HIGH);
  maxspeed = 35;
  while(1){
    mot_go(45,255,0);
    if( lidgo(tree2) == true){s=0;break;}
  }
  maxspeed=40;
  Shoot(HIGH);
  s=256;
  mot_go(-135,255,255);
  delay(1300);
  s=0;
//  while(1){
//    mot_go(-135,255,255);
//    if( sharpgo(ninetydiff) == true /*|| digitalRead(digitalRead(digitalIO[LEFTSW1]) == LOW || digitalRead(digitalIO[LEFTSW2]) == LOW)*/){s=0;break;}
//  }
  maxspeed=maxspeednorm;
  useshoot(LOW);
  //parallelleft();
  s=256;
//  while(1){
//    mot_go(180,100,127);
//    if(digitalRead(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW)){delay(250);s=0;break;}
//  }
  lidserv(0);
//  while(1){
//    mot_go(-95,150,127);
//    if( lidgo(tz1ycorner) == true || digitalRead(digitalIO[CORNERSENS]) == HIGH){s=0;break;}
//  }
  cornerdetect();
  break;
}
}
void func4(){while(1){
  digitalWrite(led[0],LOW);
  digitalWrite(led[1],LOW);
  digitalWrite(led[2],LOW);
  digitalWrite(led[3],HIGH);
  while(1){
    mot_stop();
    if(digitalRead(digitalIO[BALLSENS]) == LOW && digitalRead(digitalIO[BACKSENS1]) == HIGH && digitalRead(digitalIO[BACKSENS2]) == HIGH){delay(500);break;}  
  }
  mos(PR1,HIGH);
  mos(CATCH,HIGH);
  s=256;
  maxspeed = 50;
  while(1){
    mot_go(45,255,0);
    delay(1300);
    s=0;break;
    if(digitalRead(digitalIO[FRONTSW1]) == LOW /*&& digitalRead(digitalIO[FRONTSW2]) == LOW)*/){s=0;break;}
  }
  mot_stop();
  lidserv(90);
  s=256;
  while(1){
    mot_go(90,150,127);
    if(digitalRead(digitalIO[FRONTSW1]) == LOW && digitalRead(digitalIO[FRONTSW2]) == LOW){delay(250);s=0;break;}
  }
  mot_stop();
  maxspeed = 200;
  tz3go();
  s=256;
  maxspeed = 100;
  useshoot(HIGH);
  while(1){
    while(1){
    mot_go(5,255,127);
    if( lidgo(tree3) == true){break;}
    }
    while(1){
    mot_go(175,150,127);
    if( lidgo(tree3) == true){break;}
    }
    break;
  }
  mot_stop(); 
  Shoot(LOW);
  s=256;
  
  maxspeed = 50;
  while(1){
    mot_go(-135,255,255);
    delay(1300);
    break;
    //if(digitalRead(digitalIO[FRONTSW1]) == LOW /*&& digitalRead(digitalIO[FRONTSW2]) == LOW)*/){s=0;break;}
  }
  mot_stop();
  s=256;
  useshoot(LOW);
  maxspeed=maxspeednorm;
  while(1){
    mot_go(180,150,127);
    if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){delay(250);s=0;break;}
  }
  mot_stop();
  maxspeed = maxspeednorm;
  lidserv(0);
  useshoot(LOW);
  //parallelfront();
  tz3back();
  mot_stop();
  cornerdetect(); 
  break;
}
}

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
  maxspeed=150;
  s=256;
  while(1){
    if(digitalRead(digitalIO[CORNERSENS]) == HIGH){
      while(1){
        mot_go(90,50,127);
        if(digitalRead(digitalIO[CORNERSENS]) == LOW){break;}
      }
      while(1){
        mot_go(180,70,127);
        if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){break;}
      }
      while(1){
        mot_go(-95,50,127);
        if(digitalRead(digitalIO[CORNERSENS]) == HIGH){break;}
      }
      break;
    }
    else{
      while(1){
        mot_go(180,70,127);
        if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){break;}
      }
      while(1){
        mot_go(-95,50,127);
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
void tz2totz3(){
  s=256;
  maxspeed=170;
  mot_go(-90,52,127);delay(sddel);
mot_go(-90,52,127);delay(sddel);
mot_go(-90,62,127);delay(sddel);
mot_go(-90,62,122);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,84,123);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,84,127);delay(sddel);
mot_go(-90,84,127);delay(sddel);
mot_go(-57.2,107.07,122);delay(sddel);
mot_go(-59.38,113.88,122);delay(sddel);
mot_go(-51.04,120.88,127);delay(sddel);
mot_go(-50.08,127.78,127);delay(sddel);
mot_go(-49.3,131.89,127);delay(sddel);
mot_go(-48.73,130.38,127);delay(sddel);
mot_go(-49.3,131.89,122);delay(sddel);
mot_go(-49.3,131.89,122);delay(sddel);
mot_go(-47.54,127.4,127);delay(sddel);
mot_go(-46.89,128.76,127);delay(sddel);
mot_go(-47.54,127.4,127);delay(sddel);
mot_go(-47.6,124.58,123);delay(sddel);
mot_go(-46.93,125.94,127);delay(sddel);
mot_go(-45,130.11,127);delay(sddel);
mot_go(-45,130.11,127);delay(sddel);
mot_go(-46.19,135.79,127);delay(sddel);
mot_go(-41.57,141.68,123);delay(sddel);
mot_go(-39.02,149.31,123);delay(sddel);
mot_go(-38.07,152.43,127);delay(sddel);
mot_go(-37.16,155.6,127);delay(sddel);
mot_go(-38.32,158.05,127);delay(sddel);
mot_go(-36.59,164.4,127);delay(sddel);
mot_go(-37.38,181.21,120);delay(sddel);
mot_go(-33.34,181.95,127);delay(sddel);
mot_go(-33.69,180.28,127);delay(sddel);
mot_go(-33.86,183.05,127);delay(sddel);
mot_go(-33.86,183.05,127);delay(sddel);
mot_go(-37.35,191.21,127);delay(sddel);
mot_go(-33.86,183.05,127);delay(sddel);
mot_go(-33.86,183.05,127);delay(sddel);
mot_go(-35.39,186.46,127);delay(sddel);
mot_go(-35.39,186.46,127);delay(sddel);
mot_go(-33.86,190.26,127);delay(sddel);
mot_go(-34.35,191.38,127);delay(sddel);
mot_go(-35.33,193.67,127);delay(sddel);
mot_go(-34.35,191.38,127);delay(sddel);
mot_go(-33.69,194.7,127);delay(sddel);
mot_go(-34.99,195.3,127);delay(sddel);
mot_go(-33.53,199.14,127);delay(sddel);
mot_go(-34,207.46,127);delay(sddel);
mot_go(-33.84,211.9,127);delay(sddel);
mot_go(-33.69,216.33,127);delay(sddel);
mot_go(-33.55,220.77,127);delay(sddel);
mot_go(-33.55,220.77,127);delay(sddel);
mot_go(-32.55,223.03,122);delay(sddel);
mot_go(-31.74,228.11,123);delay(sddel);
mot_go(-33.97,229.09,127);delay(sddel);
mot_go(-32.88,235.77,123);delay(sddel);
mot_go(-33.29,236.86,127);delay(sddel);
mot_go(-34.08,246.29,123);delay(sddel);
mot_go(-34.08,246.29,127);delay(sddel);
mot_go(-34.08,246.29,127);delay(sddel);
mot_go(-34.46,247.42,127);delay(sddel);
mot_go(-36.03,244.83,123);delay(sddel);
mot_go(-36.03,244.83,123);delay(sddel);
mot_go(-35.75,246.45,127);delay(sddel);
mot_go(-36.3,243.21,127);delay(sddel);
mot_go(-36.5,248.81,127);delay(sddel);
mot_go(-36.4,246.01,127);delay(sddel);
mot_go(-36.5,248.81,127);delay(sddel);
mot_go(-37.23,251.21,127);delay(sddel);
mot_go(-37.51,249.62,127);delay(sddel);
mot_go(-37.41,255,127);delay(sddel);
mot_go(-37.41,255,127);delay(sddel);
mot_go(-38.23,252.07,123);delay(sddel);
mot_go(-38.23,252.07,123);delay(sddel);
mot_go(-37.14,255,127);delay(sddel);
mot_go(-36.87,255,127);delay(sddel);
mot_go(-36.35,255,127);delay(sddel);
mot_go(-37.91,255,127);delay(sddel);
mot_go(-38.18,255,123);delay(sddel);
mot_go(-38.25,255,127);delay(sddel);
mot_go(-36.7,255,127);delay(sddel);
mot_go(-37.54,255,127);delay(sddel);
mot_go(-38.06,255,123);delay(sddel);
mot_go(-37.54,255,127);delay(sddel);
mot_go(-37.54,255,127);delay(sddel);
mot_go(-37.21,255,127);delay(sddel);
mot_go(-36.87,255,127);delay(sddel);
mot_go(-36.45,255,127);delay(sddel);
mot_go(-35.97,255,127);delay(sddel);
mot_go(-33.8,255,122);delay(sddel);
mot_go(-33.36,255,123);delay(sddel);
mot_go(-30.05,255,127);delay(sddel);
mot_go(-28.71,255,122);delay(sddel);
mot_go(-26.93,255,127);delay(sddel);
mot_go(-26.93,255,127);delay(sddel);
mot_go(-32.11,221.96,127);delay(sddel);
mot_go(-23.33,255,127);delay(sddel);
mot_go(-22.95,255,127);delay(sddel);
mot_go(-22.95,255,127);delay(sddel);
mot_go(-20.24,255,127);delay(sddel);
mot_go(-19.04,255,122);delay(sddel);
mot_go(-18.23,255,127);delay(sddel);
mot_go(-14.51,255,127);delay(sddel);
mot_go(-12.39,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,122);delay(sddel);
mot_go(0,255,122);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(11.53,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,123);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(0,255,127);delay(sddel);
mot_go(11.53,255,123);delay(sddel);
mot_go(11.53,255,123);delay(sddel);
mot_go(21.07,233.62,123);delay(sddel);
mot_go(21.07,233.62,123);delay(sddel);
mot_go(31.5,218.16,127);delay(sddel);
mot_go(36.22,213.21,127);delay(sddel);
mot_go(44.56,185.27,127);delay(sddel);
mot_go(51.24,182.1,122);delay(sddel);
mot_go(54.75,183.67,127);delay(sddel);
mot_go(59.35,188.31,127);delay(sddel);
mot_go(59.25,183.85,123);delay(sddel);
mot_go(59.44,192.77,127);delay(sddel);
mot_go(59.44,192.77,127);delay(sddel);
mot_go(61.06,194.26,127);delay(sddel);
mot_go(60.33,197.96,127);delay(sddel);
mot_go(60.89,201.44,127);delay(sddel);
mot_go(63.68,207.51,127);delay(sddel);
mot_go(67.11,195.39,127);delay(sddel);
mot_go(73.9,201.92,123);delay(sddel);
mot_go(90,214,127);delay(sddel);
mot_go(90,216,127);delay(sddel);
mot_go(90,226,127);delay(sddel);
mot_go(90,226,127);delay(sddel);
mot_go(90,226,127);delay(sddel);
mot_go(90,232,123);delay(sddel);
mot_go(90,226,127);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,228,122);delay(sddel);
mot_go(90,228,123);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,224,127);delay(sddel);
mot_go(90,224,127);delay(sddel);
mot_go(90,228,117);delay(sddel);
mot_go(90,232,123);delay(sddel);
mot_go(90,226,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,226,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,244,122);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,234,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,232,123);delay(sddel);
mot_go(90,232,127);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,228,123);delay(sddel);
mot_go(90,226,123);delay(sddel);
mot_go(90,228,127);delay(sddel);
mot_go(90,222,127);delay(sddel);
mot_go(90,214,127);delay(sddel);
mot_go(90,214,127);delay(sddel);
mot_go(90,206,127);delay(sddel);
mot_go(90,200,123);delay(sddel);
mot_go(90,198,123);delay(sddel);
mot_go(90,198,123);delay(sddel);
mot_go(90,198,123);delay(sddel);
mot_go(90,200,123);delay(sddel);
mot_go(90,200,123);delay(sddel);
mot_go(90,204,123);delay(sddel);
mot_go(90,204,123);delay(sddel);
mot_go(90,200,127);delay(sddel);
mot_go(90,198,127);delay(sddel);
mot_go(90,204,127);delay(sddel);
mot_go(90,200,123);delay(sddel);
mot_go(90,198,120);delay(sddel);
mot_go(90,196,127);delay(sddel);
mot_go(90,188,127);delay(sddel);
mot_go(90,188,127);delay(sddel);
mot_go(90,190,127);delay(sddel);
mot_go(90,186,127);delay(sddel);
mot_go(90,186,119);delay(sddel);
mot_go(90,178,127);delay(sddel);
mot_go(90,170,123);delay(sddel);
mot_go(90,150,127);delay(sddel);
mot_go(90,134,127);delay(sddel);
mot_go(90,132,127);delay(sddel);
mot_go(90,102,127);delay(sddel);
mot_go(90,102,127);delay(sddel);
maxspeed=maxspeednorm;
s=0;
}
void arstotz1(){
  maxspeed=190;
  s=256;
  mot_go(0,50,134);delay(sddel);
mot_go(0,0,142);delay(sddel);
mot_go(0,0,142);delay(sddel);
  mot_go(0,50,133);delay(sddel);
mot_go(0,68,132);delay(sddel);
mot_go(0,50,134);delay(sddel);
mot_go(0,66,136);delay(sddel);
mot_go(0,64,143);delay(sddel);
mot_go(0,68,137);delay(sddel);
mot_go(0,66,139);delay(sddel);
mot_go(0,82,131);delay(sddel);
mot_go(0,82,133);delay(sddel);
mot_go(0,82,133);delay(sddel);
mot_go(0,88,127);delay(sddel);
mot_go(0,88,127);delay(sddel);
mot_go(0,88,168);delay(sddel);
mot_go(0,86,137);delay(sddel);
mot_go(0,88,148);delay(sddel);
mot_go(0,92,127);delay(sddel);
mot_go(0,94,127);delay(sddel);
mot_go(0,102,127);delay(sddel);
mot_go(0,94,127);delay(sddel);
mot_go(0,94,132);delay(sddel);
mot_go(0,106,132);delay(sddel);
mot_go(0,110,131);delay(sddel);
mot_go(0,114,132);delay(sddel);
mot_go(0,110,147);delay(sddel);
mot_go(0,120,127);delay(sddel);
mot_go(0,116,133);delay(sddel);
mot_go(0,122,133);delay(sddel);
mot_go(0,124,132);delay(sddel);
mot_go(0,116,127);delay(sddel);
mot_go(0,116,127);delay(sddel);
mot_go(0,124,133);delay(sddel);
mot_go(0,122,133);delay(sddel);
mot_go(0,124,132);delay(sddel);
mot_go(0,122,127);delay(sddel);
mot_go(0,122,131);delay(sddel);
mot_go(21.96,133.7,133);delay(sddel);
mot_go(0,124,133);delay(sddel);
mot_go(22.75,134.46,131);delay(sddel);
mot_go(21.21,143.74,131);delay(sddel);
mot_go(21.21,143.74,127);delay(sddel);
mot_go(21.64,135.56,133);delay(sddel);
mot_go(21.8,140.01,132);delay(sddel);
mot_go(23.3,141.55,133);delay(sddel);
mot_go(21.8,140.01,133);delay(sddel);
mot_go(24.04,142.35,133);delay(sddel);
mot_go(24.12,146.82,131);delay(sddel);
mot_go(22.22,153.39,133);delay(sddel);
mot_go(23.96,157.58,131);delay(sddel);
mot_go(22.07,159.7,127);delay(sddel);
mot_go(23.39,161.25,133);delay(sddel);
mot_go(24.03,162.05,133);delay(sddel);
mot_go(24.03,162.05,133);delay(sddel);
mot_go(27.47,169.07,133);delay(sddel);
mot_go(28.66,170.95,127);delay(sddel);
mot_go(28.66,170.95,127);delay(sddel);
mot_go(29.5,174.64,133);delay(sddel);
mot_go(27.16,170.84,132);delay(sddel);
mot_go(29.5,174.64,133);delay(sddel);
mot_go(28.93,173.67,133);delay(sddel);
mot_go(30.21,182.83,127);delay(sddel);
mot_go(29.74,177.37,131);delay(sddel);
mot_go(29.18,176.39,133);delay(sddel);
mot_go(29.18,176.39,133);delay(sddel);
mot_go(28.56,179.89,127);delay(sddel);
mot_go(28.56,179.89,133);delay(sddel);
mot_go(28.93,173.67,133);delay(sddel);
mot_go(28.26,181.65,133);delay(sddel);
mot_go(29.18,176.39,137);delay(sddel);
mot_go(28.93,173.67,131);delay(sddel);
mot_go(28.56,179.89,127);delay(sddel);
mot_go(28,178.94,132);delay(sddel);
mot_go(26.85,181.57,133);delay(sddel);
mot_go(27.7,180.71,132);delay(sddel);
mot_go(27.7,180.71,132);delay(sddel);
mot_go(26.02,186.94,131);delay(sddel);
mot_go(26.02,186.94,146);delay(sddel);
mot_go(24.34,184.39,131);delay(sddel);
mot_go(26.57,187.83,133);delay(sddel);
mot_go(24.9,185.22,133);delay(sddel);
mot_go(25.75,188.74,133);delay(sddel);
mot_go(25.49,190.55,146);delay(sddel);
mot_go(23.9,192.51,133);delay(sddel);
mot_go(23.66,194.34,127);delay(sddel);
mot_go(24.49,197.8,131);delay(sddel);
mot_go(24.49,197.8,131);delay(sddel);
mot_go(25.54,199.49,133);delay(sddel);
mot_go(25.54,199.49,133);delay(sddel);
mot_go(19.73,195.48,127);delay(sddel);
mot_go(19.54,197.36,133);delay(sddel);
mot_go(17.7,197.34,133);delay(sddel);
mot_go(16.1,201.92,132);delay(sddel);
mot_go(14.86,202.78,131);delay(sddel);
mot_go(14.04,206.16,131);delay(sddel);
mot_go(15.21,213.48,133);delay(sddel);
mot_go(0,214,131);delay(sddel);
mot_go(0,214,131);delay(sddel);
mot_go(0,214,132);delay(sddel);
mot_go(0,216,131);delay(sddel);
mot_go(0,218,131);delay(sddel);
mot_go(12.96,231.91,132);delay(sddel);
mot_go(0,228,132);delay(sddel);
mot_go(12.37,233.42,132);delay(sddel);
mot_go(14.39,233.32,133);delay(sddel);
mot_go(12.16,237.33,131);delay(sddel);
mot_go(12.16,237.33,131);delay(sddel);
mot_go(12.48,231.46,133);delay(sddel);
mot_go(12.48,231.46,133);delay(sddel);
mot_go(12.96,231.91,133);delay(sddel);
mot_go(12.85,233.85,133);delay(sddel);
mot_go(13.8,234.78,133);delay(sddel);
mot_go(13.8,234.78,134);delay(sddel);
mot_go(13.8,234.78,132);delay(sddel);
mot_go(12.37,233.42,134);delay(sddel);
mot_go(12.37,233.42,134);delay(sddel);
mot_go(13.92,241.08,131);delay(sddel);
mot_go(13.92,241.08,131);delay(sddel);
mot_go(14.27,235.26,131);delay(sddel);
mot_go(14.52,231.39,131);delay(sddel);
mot_go(14.87,233.83,127);delay(sddel);
mot_go(14.87,233.83,133);delay(sddel);
mot_go(15,231.9,132);delay(sddel);
mot_go(16.28,235.44,127);delay(sddel);
mot_go(15.68,236.81,127);delay(sddel);
mot_go(15.81,234.89,133);delay(sddel);
mot_go(14.87,233.83,133);delay(sddel);
mot_go(16.42,233.52,133);delay(sddel);
mot_go(16.42,233.52,133);delay(sddel);
mot_go(20.61,232.91,131);delay(sddel);
mot_go(20.61,232.91,133);delay(sddel);
mot_go(19.36,235.3,133);delay(sddel);
mot_go(19.36,235.3,133);delay(sddel);
mot_go(20.27,236.66,133);delay(sddel);
mot_go(21.07,233.62,132);delay(sddel);
mot_go(19.69,231.53,127);delay(sddel);
mot_go(19.69,231.53,131);delay(sddel);
mot_go(19.69,231.53,131);delay(sddel);
mot_go(19.69,231.53,133);delay(sddel);
mot_go(19.69,231.53,133);delay(sddel);
mot_go(20.03,227.77,131);delay(sddel);
mot_go(19.69,231.53,134);delay(sddel);
mot_go(20.79,231.04,133);delay(sddel);
mot_go(19.36,235.3,127);delay(sddel);
mot_go(20.61,232.91,133);delay(sddel);
mot_go(20.61,232.91,139);delay(sddel);
mot_go(19.36,235.3,133);delay(sddel);
mot_go(21.07,233.62,133);delay(sddel);
mot_go(19.69,231.53,131);delay(sddel);
mot_go(19.36,235.3,127);delay(sddel);
mot_go(20.27,236.66,133);delay(sddel);
mot_go(20.79,231.04,133);delay(sddel);
mot_go(19.36,235.3,132);delay(sddel);
mot_go(20.61,232.91,127);delay(sddel);
mot_go(19.2,237.19,133);delay(sddel);
mot_go(18.9,234.65,132);delay(sddel);
mot_go(19.36,235.3,127);delay(sddel);
mot_go(20.11,238.54,133);delay(sddel);
mot_go(18.74,236.54,146);delay(sddel);
mot_go(18.58,244.76,133);delay(sddel);
mot_go(15.75,243.13,133);delay(sddel);
mot_go(15.75,243.13,133);delay(sddel);
mot_go(18.29,248.56,131);delay(sddel);
mot_go(13.03,248.39,132);delay(sddel);
mot_go(12.63,255,133);delay(sddel);
mot_go(0,252,131);delay(sddel);
mot_go(12.43,255,127);delay(sddel);
mot_go(0,255,131);delay(sddel);
mot_go(0,255,133);delay(sddel);
mot_go(0,252,133);delay(sddel);
mot_go(0,246,127);delay(sddel);
mot_go(0,246,127);delay(sddel);
mot_go(13.35,242.55,133);delay(sddel);
mot_go(12.03,249.48,133);delay(sddel);
mot_go(13.35,242.55,131);delay(sddel);
mot_go(13.13,246.45,133);delay(sddel);
mot_go(14.26,243.51,131);delay(sddel);
mot_go(14.38,241.57,132);delay(sddel);
mot_go(14.5,239.63,133);delay(sddel);
mot_go(14.38,241.57,132);delay(sddel);
mot_go(18,252.36,131);delay(sddel);
mot_go(15.88,241.21,127);delay(sddel);
mot_go(19.47,246.06,131);delay(sddel);
mot_go(19.04,239.08,132);delay(sddel);
mot_go(19.04,239.08,132);delay(sddel);
mot_go(19.78,242.3,131);delay(sddel);
mot_go(19.2,237.19,131);delay(sddel);
mot_go(18.89,240.97,133);delay(sddel);
mot_go(19.94,240.42,131);delay(sddel);
mot_go(18.89,240.97,131);delay(sddel);
mot_go(20.11,238.54,133);delay(sddel);
mot_go(19.04,239.08,133);delay(sddel);
mot_go(19.04,239.08,133);delay(sddel);
mot_go(19.04,239.08,133);delay(sddel);
mot_go(18.43,240.33,133);delay(sddel);
mot_go(18.43,240.33,133);delay(sddel);
mot_go(18.89,240.97,127);delay(sddel);
mot_go(20.11,238.54,133);delay(sddel);
mot_go(18.89,240.97,127);delay(sddel);
mot_go(19.78,242.3,133);delay(sddel);
mot_go(18.89,240.97,131);delay(sddel);
mot_go(19.94,240.42,133);delay(sddel);
mot_go(18.58,244.76,133);delay(sddel);
mot_go(19.47,246.06,133);delay(sddel);
mot_go(18.14,244.13,132);delay(sddel);
mot_go(19.47,246.06,132);delay(sddel);
mot_go(19.47,246.06,127);delay(sddel);
mot_go(18.58,244.76,131);delay(sddel);
mot_go(18.58,244.76,132);delay(sddel);
mot_go(18.29,248.56,131);delay(sddel);
mot_go(19.78,242.3,133);delay(sddel);
mot_go(18.43,246.66,132);delay(sddel);
mot_go(19.75,248.62,134);delay(sddel);
mot_go(19.16,249.84,133);delay(sddel);
mot_go(18.29,248.56,134);delay(sddel);
mot_go(19.16,249.84,133);delay(sddel);
mot_go(19.16,249.84,133);delay(sddel);
mot_go(18.86,253.62,133);delay(sddel);
mot_go(17.73,255,131);delay(sddel);
mot_go(17.73,255,133);delay(sddel);
mot_go(18.58,255,133);delay(sddel);
mot_go(19.83,255,142);delay(sddel);
mot_go(17.59,255,133);delay(sddel);
mot_go(18.58,255,133);delay(sddel);
mot_go(18.43,255,132);delay(sddel);
mot_go(17.33,255,133);delay(sddel);
mot_go(16.78,255,127);delay(sddel);
mot_go(16.78,255,127);delay(sddel);
mot_go(17.59,255,132);delay(sddel);
mot_go(17.59,255,132);delay(sddel);
mot_go(18.16,255,127);delay(sddel);
mot_go(17.2,255,127);delay(sddel);
mot_go(17.2,255,127);delay(sddel);
mot_go(18.57,255,133);delay(sddel);
mot_go(18.84,255,133);delay(sddel);
mot_go(17.33,255,131);delay(sddel);
mot_go(18.43,255,132);delay(sddel);
mot_go(18.85,255,133);delay(sddel);
mot_go(18.85,255,133);delay(sddel);
mot_go(18.02,255,132);delay(sddel);
mot_go(18.02,255,132);delay(sddel);
mot_go(18.02,255,127);delay(sddel);
mot_go(18.43,255,131);delay(sddel);
mot_go(18.3,255,132);delay(sddel);
mot_go(17.89,255,131);delay(sddel);
mot_go(17.83,255,127);delay(sddel);
mot_go(17.07,255,127);delay(sddel);
mot_go(18.85,255,132);delay(sddel);
mot_go(18.43,255,132);delay(sddel);
mot_go(18.43,255,132);delay(sddel);
mot_go(17.83,255,127);delay(sddel);
mot_go(17.83,255,127);delay(sddel);
mot_go(17.07,255,131);delay(sddel);
mot_go(18.43,255,136);delay(sddel);
mot_go(18.43,255,133);delay(sddel);
mot_go(18.71,255,133);delay(sddel);
mot_go(17.83,255,127);delay(sddel);
mot_go(18.43,255,133);delay(sddel);
mot_go(18.43,255,133);delay(sddel);
mot_go(18.02,255,127);delay(sddel);
mot_go(18.43,255,131);delay(sddel);
mot_go(19.04,255,127);delay(sddel);
mot_go(19.04,255,127);delay(sddel);
mot_go(18.64,255,131);delay(sddel);
mot_go(18.3,255,133);delay(sddel);
mot_go(19.04,255,131);delay(sddel);
mot_go(18.43,255,133);delay(sddel);
mot_go(18.71,255,131);delay(sddel);
mot_go(19.56,255,133);delay(sddel);
mot_go(21.1,244.39,133);delay(sddel);
mot_go(24.68,244.31,131);delay(sddel);
mot_go(24.68,244.31,131);delay(sddel);
mot_go(31.41,222.61,127);delay(sddel);
mot_go(39.45,204.61,127);delay(sddel);
mot_go(48.42,189.84,127);delay(sddel);
mot_go(51.43,202.09,133);delay(sddel);
mot_go(63.43,187.83,132);delay(sddel);
mot_go(62.9,193.2,131);delay(sddel);
mot_go(63.97,191.42,131);delay(sddel);
mot_go(63.96,195.89,131);delay(sddel);
mot_go(62.9,193.2,132);delay(sddel);
mot_go(63.43,196.77,132);delay(sddel);
mot_go(63.43,196.77,133);delay(sddel);
mot_go(63.96,195.89,133);delay(sddel);
mot_go(67.2,190.92,131);delay(sddel);
mot_go(65.27,195.98,127);delay(sddel);
mot_go(66.88,193.55,139);delay(sddel);
mot_go(70.27,195.48,138);delay(sddel);
mot_go(70.27,195.48,137);delay(sddel);
mot_go(74.8,190.67,134);delay(sddel);
mot_go(74.95,192.6,133);delay(sddel);
mot_go(90,178,131);delay(sddel);
mot_go(90,178,131);delay(sddel);
mot_go(90,184,131);delay(sddel);
mot_go(90,180,127);delay(sddel);
mot_go(90,184,127);delay(sddel);
mot_go(90,186,133);delay(sddel);
mot_go(90,184,133);delay(sddel);
mot_go(90,188,132);delay(sddel);
mot_go(90,180,132);delay(sddel);
mot_go(90,180,127);delay(sddel);
mot_go(90,178,133);delay(sddel);
mot_go(90,162,133);delay(sddel);
mot_go(90,162,133);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,144,132);delay(sddel);
mot_go(90,140,132);delay(sddel);
mot_go(90,140,127);delay(sddel);
mot_go(90,134,133);delay(sddel);
mot_go(90,134,147);delay(sddel);
mot_go(90,138,132);delay(sddel);
mot_go(90,140,133);delay(sddel);
mot_go(90,140,133);delay(sddel);
mot_go(90,142,127);delay(sddel);
mot_go(90,142,127);delay(sddel);
mot_go(90,144,132);delay(sddel);
mot_go(90,144,133);delay(sddel);
mot_go(90,148,131);delay(sddel);
mot_go(90,144,131);delay(sddel);
mot_go(90,144,132);delay(sddel);
mot_go(90,148,127);delay(sddel);
mot_go(90,150,133);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,150,133);delay(sddel);
mot_go(90,148,132);delay(sddel);
mot_go(90,144,127);delay(sddel);
mot_go(90,144,133);delay(sddel);
mot_go(90,142,133);delay(sddel);
mot_go(90,144,131);delay(sddel);
mot_go(90,142,131);delay(sddel);
mot_go(90,144,131);delay(sddel);
mot_go(90,142,133);delay(sddel);
mot_go(90,142,133);delay(sddel);
mot_go(90,148,133);delay(sddel);
mot_go(90,144,132);delay(sddel);
mot_go(90,144,127);delay(sddel);
mot_go(90,142,133);delay(sddel);
mot_go(90,142,133);delay(sddel);
mot_go(90,142,127);delay(sddel);
mot_go(90,134,133);delay(sddel);
mot_go(90,132,133);delay(sddel);
mot_go(90,132,132);delay(sddel);
mot_go(90,132,132);delay(sddel);
mot_go(90,132,131);delay(sddel);
mot_go(90,132,133);delay(sddel);
mot_go(90,130,127);delay(sddel);
mot_go(90,130,131);delay(sddel);
mot_go(90,126,133);delay(sddel);
mot_go(90,126,131);delay(sddel);
mot_go(90,126,131);delay(sddel);
mot_go(90,122,127);delay(sddel);
mot_go(90,122,127);delay(sddel);
mot_go(90,120,133);delay(sddel);
mot_go(90,120,136);delay(sddel);
mot_go(90,116,132);delay(sddel);
mot_go(90,114,133);delay(sddel);
mot_go(90,114,127);delay(sddel);
mot_go(90,106,127);delay(sddel);
mot_go(90,104,133);delay(sddel);
mot_go(90,104,132);delay(sddel);
mot_go(90,104,132);delay(sddel);
mot_go(90,102,133);delay(sddel);
mot_go(90,88,131);delay(sddel);
mot_go(90,56,133);delay(sddel);
maxspeed=maxspeednorm;
s=0;
}
void tz3go(){
 maxspeed=200;
 s=256;
 mot_go(0,52,127);delay(sddel);
mot_go(0,0,127);delay(sddel);
mot_go(0,58,127);delay(sddel);
mot_go(0,68,123);delay(sddel);
mot_go(0,78,127);delay(sddel);
mot_go(0,86,127);delay(sddel);
mot_go(0,88,127);delay(sddel);
mot_go(0,88,127);delay(sddel);
mot_go(29.6,101.21,123);delay(sddel);
mot_go(29.74,112.87,123);delay(sddel);
mot_go(29.74,112.87,123);delay(sddel);
mot_go(28.69,120.83,122);delay(sddel);
mot_go(28.89,132.48,122);delay(sddel);
mot_go(26.21,144.9,127);delay(sddel);
mot_go(23.39,161.25,123);delay(sddel);
mot_go(23.75,163.88,127);delay(sddel);
mot_go(24.44,169.16,127);delay(sddel);
mot_go(26.57,169.94,127);delay(sddel);
mot_go(27.43,178.01,127);delay(sddel);
mot_go(28.03,174.47,127);delay(sddel);
mot_go(28.03,174.47,127);delay(sddel);
mot_go(27.43,178.01,127);delay(sddel);
mot_go(27.47,169.07,127);delay(sddel);
mot_go(26.57,169.94,127);delay(sddel);
mot_go(26.87,168.15,127);delay(sddel);
mot_go(26.27,171.73,127);delay(sddel);
mot_go(27.14,179.79,127);delay(sddel);
mot_go(28.03,174.47,127);delay(sddel);
mot_go(26.86,172.63,127);delay(sddel);
mot_go(26.86,172.63,127);delay(sddel);
mot_go(24.55,178.1,127);delay(sddel);
mot_go(23.63,174.64,133);delay(sddel);
mot_go(23.63,174.64,133);delay(sddel);
mot_go(24.6,182.57,133);delay(sddel);
mot_go(21.69,189.41,127);delay(sddel);
mot_go(22.38,183.85,127);delay(sddel);
mot_go(21.25,193.13,127);delay(sddel);
mot_go(21.47,191.27,127);delay(sddel);
mot_go(21.22,182.36,127);delay(sddel);
mot_go(20.99,184.23,127);delay(sddel);
mot_go(20.7,192.42,127);delay(sddel);
mot_go(20.34,189.84,127);delay(sddel);
mot_go(20.34,189.84,127);delay(sddel);
mot_go(20.34,189.84,127);delay(sddel);
mot_go(19.54,197.36,127);delay(sddel);
mot_go(21.25,193.13,127);delay(sddel);
mot_go(20.7,192.42,122);delay(sddel);
mot_go(20.62,198.74,123);delay(sddel);
mot_go(20.91,190.55,127);delay(sddel);
mot_go(22.35,194.62,123);delay(sddel);
mot_go(19.18,194.81,122);delay(sddel);
mot_go(18.99,196.7,127);delay(sddel);
mot_go(19.18,194.81,127);delay(sddel);
mot_go(18.99,196.7,123);delay(sddel);
mot_go(18.99,196.7,123);delay(sddel);
mot_go(18.06,193.54,127);delay(sddel);
mot_go(20.14,191.72,122);delay(sddel);
mot_go(19.73,195.48,122);delay(sddel);
mot_go(19.34,199.25,127);delay(sddel);
mot_go(20.28,196.16,127);delay(sddel);
mot_go(19.73,195.48,127);delay(sddel);
mot_go(19.54,197.36,127);delay(sddel);
mot_go(18.8,198.6,127);delay(sddel);
mot_go(18.62,200.49,122);delay(sddel);
mot_go(17.53,199.25,123);delay(sddel);
mot_go(16.1,201.92,123);delay(sddel);
mot_go(16.48,204.4,123);delay(sddel);
mot_go(16.48,204.4,123);delay(sddel);
mot_go(15.64,207.69,127);delay(sddel);
mot_go(16.39,212.64,123);delay(sddel);
mot_go(17.6,218.22,127);delay(sddel);
mot_go(19.58,220.77,123);delay(sddel);
mot_go(14.64,229.45,123);delay(sddel);
mot_go(14.52,231.39,127);delay(sddel);
mot_go(15,231.9,127);delay(sddel);
mot_go(14.64,229.45,123);delay(sddel);
mot_go(14.64,229.45,123);delay(sddel);
mot_go(14.64,229.45,127);delay(sddel);
mot_go(14.64,229.45,127);delay(sddel);
mot_go(19.86,229.65,122);delay(sddel);
mot_go(14.87,233.83,127);delay(sddel);
mot_go(18.91,228.32,122);delay(sddel);
mot_go(18.75,230.22,127);delay(sddel);
mot_go(19.69,231.53,127);delay(sddel);
mot_go(15.68,236.81,127);delay(sddel);
mot_go(15.81,234.89,127);delay(sddel);
mot_go(14.74,235.76,123);delay(sddel);
mot_go(14.74,235.76,127);delay(sddel);
mot_go(14.5,239.63,123);delay(sddel);
mot_go(14.5,239.63,123);delay(sddel);
mot_go(13.8,234.78,123);delay(sddel);
mot_go(13.92,241.08,122);delay(sddel);
mot_go(14.38,241.57,127);delay(sddel);
mot_go(14.26,243.51,127);delay(sddel);
mot_go(14.38,241.57,123);delay(sddel);
mot_go(14.04,247.39,127);delay(sddel);
mot_go(14.26,243.51,127);delay(sddel);
mot_go(15.17,244.52,127);delay(sddel);
mot_go(15.17,244.52,123);delay(sddel);
mot_go(15.38,248.91,127);delay(sddel);
mot_go(15.38,248.91,127);delay(sddel);
mot_go(15.62,245.05,123);delay(sddel);
mot_go(15.82,249.45,127);delay(sddel);
mot_go(16.26,250,127);delay(sddel);
mot_go(15.75,243.13,127);delay(sddel);
mot_go(15.38,248.91,123);delay(sddel);
mot_go(16.26,250,127);delay(sddel);
mot_go(15.82,249.45,127);delay(sddel);
mot_go(16.52,246.16,123);delay(sddel);
mot_go(16.26,250,123);delay(sddel);
mot_go(15.57,253.3,127);delay(sddel);
mot_go(15.57,253.3,127);delay(sddel);
mot_go(16.2,243.68,127);delay(sddel);
mot_go(16.34,241.76,127);delay(sddel);
mot_go(16.2,243.68,127);delay(sddel);
mot_go(16.52,246.16,127);delay(sddel);
mot_go(16.65,244.25,123);delay(sddel);
mot_go(16.52,246.16,127);delay(sddel);
mot_go(16.13,251.92,127);delay(sddel);
mot_go(16.65,244.25,127);delay(sddel);
mot_go(16.65,244.25,127);delay(sddel);
mot_go(17.41,247.33,127);delay(sddel);
mot_go(16.26,250,122);delay(sddel);
mot_go(17.57,251.75,123);delay(sddel);
mot_go(18,252.36,122);delay(sddel);
mot_go(23.96,255,122);delay(sddel);
mot_go(21.63,249.58,127);delay(sddel);
mot_go(20.61,250,127);delay(sddel);
mot_go(20.67,243.68,127);delay(sddel);
mot_go(21.27,242.53,123);delay(sddel);
mot_go(21.53,234.35,127);delay(sddel);
mot_go(21.53,234.35,127);delay(sddel);
mot_go(21.62,238.81,123);delay(sddel);
mot_go(22.33,242.16,123);delay(sddel);
mot_go(23.01,245.54,123);delay(sddel);
mot_go(22.06,250.32,127);delay(sddel);
mot_go(21.46,251.44,123);delay(sddel);
mot_go(22.06,250.32,123);delay(sddel);
mot_go(22.06,250.32,127);delay(sddel);
mot_go(22.58,244.77,127);delay(sddel);
mot_go(22.88,236.62,127);delay(sddel);
mot_go(23.33,237.4,127);delay(sddel);
mot_go(23.33,237.4,127);delay(sddel);
mot_go(23.07,234.78,127);delay(sddel);
mot_go(23.46,231.1,122);delay(sddel);
mot_go(23.26,232.94,127);delay(sddel);
mot_go(23.86,227.44,122);delay(sddel);
mot_go(21.89,230.63,127);delay(sddel);
mot_go(22.35,231.39,123);delay(sddel);
mot_go(23.46,231.1,123);delay(sddel);
mot_go(23.07,234.78,122);delay(sddel);
mot_go(22.88,236.62,127);delay(sddel);
mot_go(22.33,242.16,123);delay(sddel);
mot_go(22.33,242.16,123);delay(sddel);
mot_go(22.33,242.16,123);delay(sddel);
mot_go(21.45,240.67,123);delay(sddel);
mot_go(22.77,242.92,127);delay(sddel);
mot_go(22.15,244.01,127);delay(sddel);
mot_go(21.97,245.86,127);delay(sddel);
mot_go(22.15,244.01,123);delay(sddel);
mot_go(21.27,242.53,127);delay(sddel);
mot_go(21.27,242.53,123);delay(sddel);
mot_go(21.1,244.39,127);delay(sddel);
mot_go(21.27,242.53,123);delay(sddel);
mot_go(21.27,242.53,123);delay(sddel);
mot_go(22.33,242.16,127);delay(sddel);
mot_go(22.33,242.16,127);delay(sddel);
mot_go(21.27,242.53,123);delay(sddel);
mot_go(20.67,243.68,127);delay(sddel);
mot_go(20.61,250,123);delay(sddel);
mot_go(22.15,244.01,127);delay(sddel);
mot_go(22.58,244.77,123);delay(sddel);
mot_go(22.77,242.92,127);delay(sddel);
mot_go(22.33,242.16,127);delay(sddel);
mot_go(21.62,238.81,122);delay(sddel);
mot_go(21.62,238.81,122);delay(sddel);
mot_go(22.35,231.39,127);delay(sddel);
mot_go(22.54,229.54,123);delay(sddel);
mot_go(23.27,217.71,127);delay(sddel);
mot_go(24.27,223.79,127);delay(sddel);
mot_go(26.33,220.93,127);delay(sddel);
mot_go(27.26,222.73,127);delay(sddel);
mot_go(29.05,226.5,127);delay(sddel);
mot_go(27.47,225.42,127);delay(sddel);
mot_go(25.69,235.26,127);delay(sddel);
mot_go(26.33,220.93,127);delay(sddel);
mot_go(26.08,209.3,127);delay(sddel);
mot_go(26.04,195.89,127);delay(sddel);
mot_go(26.04,195.89,127);delay(sddel);
mot_go(33.06,150.33,127);delay(sddel);
mot_go(42.98,120.28,127);delay(sddel);
mot_go(54.46,86.02,127);delay(sddel);
mot_go(90,58,127);delay(sddel);
s=0;
maxspeed=maxspeednorm; 
}
void tz3back(){
  maxspeed=255;
  s=256;
  mot_go(-90,50,123);delay(sddel);
mot_go(0,0,127);delay(sddel);
mot_go(-90,52,127);delay(sddel);
mot_go(-90,56,127);delay(sddel);
mot_go(-90,56,127);delay(sddel);
mot_go(-90,74,127);delay(sddel);
mot_go(-90,78,127);delay(sddel);
mot_go(-90,74,127);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,82,123);delay(sddel);
mot_go(-90,84,127);delay(sddel);
mot_go(-90,94,127);delay(sddel);
mot_go(-90,90,127);delay(sddel);
mot_go(-90,94,127);delay(sddel);
mot_go(-90,92,127);delay(sddel);
mot_go(-90,88,127);delay(sddel);
mot_go(-90,98,127);delay(sddel);
mot_go(-90,98,127);delay(sddel);
mot_go(-90,98,127);delay(sddel);
mot_go(-90,102,127);delay(sddel);
mot_go(-90,106,127);delay(sddel);
mot_go(-90,106,127);delay(sddel);
mot_go(-90,106,127);delay(sddel);
mot_go(-90,110,127);delay(sddel);
mot_go(-90,110,123);delay(sddel);
mot_go(-90,112,127);delay(sddel);
mot_go(-90,112,123);delay(sddel);
mot_go(-90,110,127);delay(sddel);
mot_go(-90,110,127);delay(sddel);
mot_go(-90,112,127);delay(sddel);
mot_go(-90,108,127);delay(sddel);
mot_go(-90,110,127);delay(sddel);
mot_go(-117,118.96,127);delay(sddel);
mot_go(-118.77,116.36,127);delay(sddel);
mot_go(-117.47,112.71,127);delay(sddel);
mot_go(-118.37,113.65,123);delay(sddel);
mot_go(-116.57,111.8,127);delay(sddel);
mot_go(-90,100,127);delay(sddel);
mot_go(-90,102,127);delay(sddel);
mot_go(-90,110,123);delay(sddel);
mot_go(-90,110,123);delay(sddel);
mot_go(-90,106,122);delay(sddel);
mot_go(-90,106,127);delay(sddel);
mot_go(-90,100,123);delay(sddel);
mot_go(-90,108,127);delay(sddel);
mot_go(-90,110,123);delay(sddel);
mot_go(-90,108,127);delay(sddel);
mot_go(-90,108,123);delay(sddel);
mot_go(-90,112,127);delay(sddel);
mot_go(-90,116,127);delay(sddel);
mot_go(-90,116,127);delay(sddel);
mot_go(-90,116,127);delay(sddel);
mot_go(-90,118,122);delay(sddel);
mot_go(-90,120,127);delay(sddel);
mot_go(-90,120,127);delay(sddel);
mot_go(-90,122,127);delay(sddel);
mot_go(-90,120,123);delay(sddel);
mot_go(-90,122,123);delay(sddel);
mot_go(-90,126,123);delay(sddel);
mot_go(-90,130,127);delay(sddel);
mot_go(-90,136,127);delay(sddel);
mot_go(-90,136,127);delay(sddel);
mot_go(-90,140,127);delay(sddel);
mot_go(-90,148,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,144,123);delay(sddel);
mot_go(-90,144,127);delay(sddel);
mot_go(-90,144,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,144,127);delay(sddel);
mot_go(-90,144,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,146,123);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,146,123);delay(sddel);
mot_go(-90,152,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,172,127);delay(sddel);
mot_go(-90,168,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,122);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,166,123);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,174,120);delay(sddel);
mot_go(-90,162,122);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,122);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,164,123);delay(sddel);
mot_go(-90,168,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,168,127);delay(sddel);
mot_go(-90,166,123);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,166,123);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,164,123);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,158,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,172,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,168,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,172,127);delay(sddel);
mot_go(-90,176,123);delay(sddel);
mot_go(-90,172,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,123);delay(sddel);
mot_go(-90,174,123);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,123);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,123);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,174,122);delay(sddel);
mot_go(-90,174,123);delay(sddel);
mot_go(-90,172,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,144,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,184,122);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,176,123);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,122);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,176,122);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,123);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,123);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,182,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,184,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,123);delay(sddel);
mot_go(-90,180,123);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,176,120);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,123);delay(sddel);
mot_go(-90,176,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,180,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,174,127);delay(sddel);
mot_go(-90,164,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,166,127);delay(sddel);
mot_go(-90,148,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,156,123);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,162,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,156,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,152,127);delay(sddel);
mot_go(-90,148,127);delay(sddel);
mot_go(-90,148,127);delay(sddel);
mot_go(-90,146,123);delay(sddel);
mot_go(-90,146,123);delay(sddel);
mot_go(-90,136,127);delay(sddel);
mot_go(-90,118,123);delay(sddel);
mot_go(-90,102,127);delay(sddel);
mot_go(-90,80,127);delay(sddel);
maxspeed=maxspeednorm;
s=0;  
}

