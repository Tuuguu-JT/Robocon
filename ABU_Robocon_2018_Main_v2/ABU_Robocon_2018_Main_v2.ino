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
int sddel=11;
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
int tree1 = 330;
int tree2 = 510;
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
  lidserv(90);
  s=0;
  while(1){
    mot_go(10,255,127);
    if( lidgo(tz1x) == true ) {s=0;break;}
  }
  mot_stop();
  maxspeed=maxspeednorm;
  lidserv(0);
  while(1){
    mot_go(90,255,127);
    if( lidgo(tz1y) == true ) {s=0;break;}
  }
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
  lidserv(0);
  while(1){
    mot_go(-90,150,127);
    if( lidgo(tz1ycorner) == true ) {s=0;break;}
  }
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
  movesd2to3();
  s=0;
  while(1){
    mot_go(90,150,127);
    if(digitalRead(digitalIO[FRONTSW1]) == LOW || digitalRead(digitalIO[FRONTSW2]) == LOW){delay(250);s=0;break;}
  }
  maxspeed = 100;
  useshoot(HIGH);
  while(1){
    mot_go(5,255,127);
    if( lidgo(tree2) == true){break;}
  }
  mot_stop(); 
  Shoot(HIGH);
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
    if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){delay(500);s=0;break;}
  }
  mot_stop();
  maxspeed = maxspeednorm;
  lidserv(0);
  useshoot(LOW);
  //parallelfront();
  while(1){
    mot_go(-90,255,127);
    if( lidgo(tz3y) == true || digitalRead(digitalIO[CORNERSENS]) == HIGH){break;}
  }
  mot_stop();
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
  while(1){
    mot_go(5,255,127);
    if( lidgo(tz3x) == true ){s=0;break;}
  }
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
    delay(250);
    break;
    //if(digitalRead(digitalIO[LEFTSW1]) == LOW && digitalRead(digitalIO[LEFTSW2]) == LOW){delay(500);s=0;break;}
  }
  mot_stop();
  maxspeed = maxspeednorm;
  lidserv(0);
  useshoot(LOW);
  //parallelfront();
  while(1){
    mot_go(-90,255,127);
    if( lidgo(tz3y) == true || digitalRead(digitalIO[CORNERSENS]) == HIGH){break;}
  }
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
  if(dist < (range+10) && dist > (range-10)){
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
void movesd2to3(){
  mot_go(-90,54,127);delay(sddel);
mot_go(-90,54,127);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,82,127);delay(sddel);
mot_go(-90,94,127);delay(sddel);
mot_go(-90,98,127);delay(sddel);
mot_go(-90,102,127);delay(sddel);
mot_go(-90,112,127);delay(sddel);
mot_go(-90,120,127);delay(sddel);
mot_go(-90,122,127);delay(sddel);
mot_go(-90,138,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,152,127);delay(sddel);
mot_go(-90,152,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,154,127);delay(sddel);
mot_go(-90,152,127);delay(sddel);
mot_go(-90,148,127);delay(sddel);
mot_go(-90,148,127);delay(sddel);
mot_go(-90,152,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,146,127);delay(sddel);
mot_go(-90,144,127);delay(sddel);
mot_go(-90,146,123);delay(sddel);
mot_go(-90,138,127);delay(sddel);
mot_go(-69.08,145.6,132);delay(sddel);
mot_go(-66.9,147.85,127);delay(sddel);
mot_go(-64.47,148.5,127);delay(sddel);
mot_go(-62.42,151.18,127);delay(sddel);
mot_go(-64.11,151.17,123);delay(sddel);
mot_go(-55.53,162.54,123);delay(sddel);
mot_go(-55.35,165.32,123);delay(sddel);
mot_go(-49.95,180.28,123);delay(sddel);
mot_go(-49.95,180.28,123);delay(sddel);
mot_go(-48.58,181.37,122);delay(sddel);
mot_go(-48.11,182.7,123);delay(sddel);
mot_go(-48.58,181.37,127);delay(sddel);
mot_go(-48.99,182.88,127);delay(sddel);
mot_go(-48.11,182.7,123);delay(sddel);
mot_go(-50.84,180.54,123);delay(sddel);
mot_go(-50.36,181.81,127);delay(sddel);
mot_go(-49.4,184.39,122);delay(sddel);
mot_go(-51.15,184.91,123);delay(sddel);
mot_go(-52.65,191.21,127);delay(sddel);
mot_go(-52.65,191.21,127);delay(sddel);
mot_go(-51.91,188.04,127);delay(sddel);
mot_go(-55.8,188.61,127);delay(sddel);
mot_go(-57.86,184.23,127);delay(sddel);
mot_go(-57.86,184.23,127);delay(sddel);
mot_go(-59.79,182.83,123);delay(sddel);
mot_go(-59.47,181.11,127);delay(sddel);
mot_go(-59.15,179.39,127);delay(sddel);
mot_go(-58.06,181.47,127);delay(sddel);
mot_go(-56.82,186.39,127);delay(sddel);
mot_go(-52.07,195.23,127);delay(sddel);
mot_go(-53.37,194.4,127);delay(sddel);
mot_go(-53.37,194.4,127);delay(sddel);
mot_go(-53.01,192.8,127);delay(sddel);
mot_go(-52.78,198.4,123);delay(sddel);
mot_go(-52.33,199.62,122);delay(sddel);
mot_go(-52.43,196.81,127);delay(sddel);
mot_go(-52.33,199.62,123);delay(sddel);
mot_go(-52.07,195.23,123);delay(sddel);
mot_go(-49.59,194.37,123);delay(sddel);
mot_go(-54.32,192.04,122);delay(sddel);
mot_go(-52.43,196.81,127);delay(sddel);
mot_go(-53.37,194.4,123);delay(sddel);
mot_go(-52.07,195.23,122);delay(sddel);
mot_go(-52.07,195.23,122);delay(sddel);
mot_go(-51.61,196.47,127);delay(sddel);
mot_go(-52.43,196.81,123);delay(sddel);
mot_go(-51.52,199.28,127);delay(sddel);
mot_go(-52.33,199.62,122);delay(sddel);
mot_go(-51.25,207.71,122);delay(sddel);
mot_go(-49.92,214.34,123);delay(sddel);
mot_go(-49.06,219.75,127);delay(sddel);
mot_go(-49.4,221.27,123);delay(sddel);
mot_go(-48.81,233.86,123);delay(sddel);
mot_go(-48.81,233.86,123);delay(sddel);
mot_go(-48.08,236.52,123);delay(sddel);
mot_go(-47.73,237.86,122);delay(sddel);
mot_go(-46.33,243.31,119);delay(sddel);
mot_go(-44.02,247.52,114);delay(sddel);
mot_go(-42.09,250.64,110);delay(sddel);
mot_go(-41.1,249.48,109);delay(sddel);
mot_go(-41.78,252.13,106);delay(sddel);
mot_go(-39.63,255,105);delay(sddel);
mot_go(-37.95,253.65,102);delay(sddel);
mot_go(-37.41,255,100);delay(sddel);
mot_go(-37.76,255,97);delay(sddel);
mot_go(-37.76,255,97);delay(sddel);
mot_go(-37.14,255,101);delay(sddel);
mot_go(-37.41,255,104);delay(sddel);
mot_go(-38.52,250.5,101);delay(sddel);
mot_go(-38.31,254.88,102);delay(sddel);
mot_go(-39.03,244.57,97);delay(sddel);
mot_go(-40.29,241.23,96);delay(sddel);
mot_go(-40.35,244.05,96);delay(sddel);
mot_go(-41.44,250.8,99);delay(sddel);
mot_go(-41.44,250.8,99);delay(sddel);
mot_go(-41.1,249.48,104);delay(sddel);
mot_go(-41.1,249.48,104);delay(sddel);
mot_go(-42.4,249.16,102);delay(sddel);
mot_go(-40.21,254.03,96);delay(sddel);
mot_go(-40.8,250.99,94);delay(sddel);
mot_go(-39.35,255,92);delay(sddel);
mot_go(-39.35,255,90);delay(sddel);
mot_go(-38.16,249.26,82);delay(sddel);
mot_go(-38.16,249.26,90);delay(sddel);
mot_go(-38.16,249.26,90);delay(sddel);
mot_go(-36.96,242.8,91);delay(sddel);
mot_go(-36.4,246.01,91);delay(sddel);
mot_go(-35.75,246.45,94);delay(sddel);
mot_go(-35.33,252.49,92);delay(sddel);
mot_go(-35.07,254.13,88);delay(sddel);
mot_go(-35.39,255,87);delay(sddel);
mot_go(-33.94,250.73,92);delay(sddel);
mot_go(-30.49,248.34,95);delay(sddel);
mot_go(-31.12,247.64,90);delay(sddel);
mot_go(-28.39,252.36,88);delay(sddel);
mot_go(-28.39,252.36,90);delay(sddel);
mot_go(-26.95,255,90);delay(sddel);
mot_go(-26.95,255,90);delay(sddel);
mot_go(-26,255,90);delay(sddel);
mot_go(-24.83,255,91);delay(sddel);
mot_go(-22.57,255,91);delay(sddel);
mot_go(-21.8,255,81);delay(sddel);
mot_go(-21.41,255,92);delay(sddel);
mot_go(-20.24,255,90);delay(sddel);
mot_go(-20.24,255,95);delay(sddel);
mot_go(-21.41,255,95);delay(sddel);
mot_go(-21.41,255,95);delay(sddel);
mot_go(-22.57,255,122);delay(sddel);
mot_go(-22.57,255,122);delay(sddel);
mot_go(-21.8,255,127);delay(sddel);
mot_go(-23.33,255,127);delay(sddel);
mot_go(-25.57,255,127);delay(sddel);
mot_go(-25.2,255,127);delay(sddel);
mot_go(-27.01,255,123);delay(sddel);
mot_go(-28.07,255,127);delay(sddel);
mot_go(-28.77,255,127);delay(sddel);
mot_go(-28.42,255,120);delay(sddel);
mot_go(-29.79,255,110);delay(sddel);
mot_go(-30.13,255,106);delay(sddel);
mot_go(-30.13,255,106);delay(sddel);
mot_go(-30.13,255,94);delay(sddel);
mot_go(-30.13,255,87);delay(sddel);
mot_go(-29.45,255,92);delay(sddel);
mot_go(-28.77,255,92);delay(sddel);
mot_go(-28.42,255,88);delay(sddel);
mot_go(-28.42,255,88);delay(sddel);
mot_go(-28.07,255,92);delay(sddel);
mot_go(-28.77,255,95);delay(sddel);
mot_go(-29.45,255,95);delay(sddel);
mot_go(-29.45,255,95);delay(sddel);
mot_go(-29.45,255,95);delay(sddel);
mot_go(-30.8,255,100);delay(sddel);
mot_go(-30.8,255,100);delay(sddel);
mot_go(-30.9,255,100);delay(sddel);
mot_go(-31.23,255,108);delay(sddel);
mot_go(-31.23,255,113);delay(sddel);
mot_go(-31.78,255,114);delay(sddel);
mot_go(-32.09,255,118);delay(sddel);
mot_go(-33.8,255,118);delay(sddel);
mot_go(-32.92,255,115);delay(sddel);
mot_go(-32.92,255,115);delay(sddel);
mot_go(-34.45,255,106);delay(sddel);
mot_go(-34.45,255,106);delay(sddel);
mot_go(-35.91,255,102);delay(sddel);
mot_go(-35.26,255,100);delay(sddel);
mot_go(-35.12,255,96);delay(sddel);
mot_go(-34.8,255,96);delay(sddel);
mot_go(-35.02,255,96);delay(sddel);
mot_go(-34.47,255,94);delay(sddel);
mot_go(-34.02,255,95);delay(sddel);
mot_go(-32.81,255,94);delay(sddel);
mot_go(-32.69,255,94);delay(sddel);
mot_go(-32.26,255,96);delay(sddel);
mot_go(-32.26,255,96);delay(sddel);
mot_go(-31.03,255,96);delay(sddel);
mot_go(-28.77,255,106);delay(sddel);
mot_go(-26.38,255,105);delay(sddel);
mot_go(-23.71,255,109);delay(sddel);
mot_go(-21.8,255,108);delay(sddel);
mot_go(-20.24,255,102);delay(sddel);
mot_go(-18.23,255,90);delay(sddel);
mot_go(-17.42,255,90);delay(sddel);
mot_go(-16.18,255,91);delay(sddel);
mot_go(-16.18,255,91);delay(sddel);
mot_go(-11.53,255,82);delay(sddel);
mot_go(-11.09,255,81);delay(sddel);
mot_go(0,255,91);delay(sddel);
mot_go(0,255,90);delay(sddel);
mot_go(0,255,90);delay(sddel);
mot_go(0,255,90);delay(sddel);
mot_go(0,242,90);delay(sddel);
mot_go(0,224,87);delay(sddel);
mot_go(0,208,90);delay(sddel);
mot_go(0,208,90);delay(sddel);
mot_go(0,184,95);delay(sddel);
mot_go(0,184,95);delay(sddel);
mot_go(0,194,95);delay(sddel);
mot_go(0,194,94);delay(sddel);
mot_go(0,206,90);delay(sddel);
mot_go(0,214,94);delay(sddel);
mot_go(0,214,91);delay(sddel);
mot_go(0,212,92);delay(sddel);
mot_go(0,214,97);delay(sddel);
mot_go(0,214,97);delay(sddel);
mot_go(0,212,115);delay(sddel);
mot_go(0,214,127);delay(sddel);
mot_go(0,216,127);delay(sddel);
mot_go(0,214,127);delay(sddel);
mot_go(0,216,123);delay(sddel);
mot_go(0,216,123);delay(sddel);
mot_go(0,212,122);delay(sddel);
mot_go(0,218,123);delay(sddel);
mot_go(0,216,127);delay(sddel);
mot_go(0,216,127);delay(sddel);
mot_go(0,214,127);delay(sddel);
mot_go(0,212,127);delay(sddel);
mot_go(0,194,127);delay(sddel);
mot_go(0,194,127);delay(sddel);
mot_go(0,188,127);delay(sddel);
mot_go(0,188,127);delay(sddel);
mot_go(0,184,123);delay(sddel);
mot_go(0,188,127);delay(sddel);
mot_go(0,190,127);delay(sddel);
mot_go(0,194,127);delay(sddel);
mot_go(0,188,127);delay(sddel);
mot_go(0,188,127);delay(sddel);
mot_go(0,188,127);delay(sddel);
mot_go(0,190,127);delay(sddel);
mot_go(0,190,123);delay(sddel);
mot_go(0,208,123);delay(sddel);
mot_go(0,208,123);delay(sddel);
mot_go(0,214,127);delay(sddel);
mot_go(0,222,127);delay(sddel);
mot_go(0,218,127);delay(sddel);
mot_go(0,216,127);delay(sddel);
mot_go(0,194,127);delay(sddel);
mot_go(0,186,123);delay(sddel);
mot_go(0,186,123);delay(sddel);
mot_go(0,180,123);delay(sddel);
mot_go(0,176,127);delay(sddel);
mot_go(0,162,127);delay(sddel);
mot_go(0,150,127);delay(sddel);
mot_go(0,150,127);delay(sddel);
mot_go(0,142,127);delay(sddel);
mot_go(0,140,127);delay(sddel);
mot_go(0,116,127);delay(sddel);
mot_go(0,116,127);delay(sddel);
mot_go(0,88,108);delay(sddel);
mot_go(0,88,108);delay(sddel);
mot_go(0,70,97);delay(sddel);
}

