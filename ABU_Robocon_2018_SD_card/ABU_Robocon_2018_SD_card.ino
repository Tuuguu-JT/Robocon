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
#include <SD.h>
#include <SPI.h>
/*------------------------SD------------------------*/
const int chipSelect = 7;
int SDdata=0;
String sddaat[4];
String SDstring = "";
File dataFile;
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
#define LIFT 1
#define CATCH 2
#define LED 3
#define PR1 4
#define PR2 5
byte mosdata = B00000000;
/*--------ANALOG------*/
#define FRONTSHARP 0
#define LEFTSHARP 1
/*--------------------------SHARP----------------------------*/
/*---------------------------LIDAR-------------------------*/

/*------------------------Serial Variable------------------------*/
byte serAngle,serZ;
double angle,Z;
double fangle;
double x,y;
double oldx,oldy;
double r;
byte serr;
byte maxspeed=100;
byte ch[11]={0,0,0,0,0,0,0,0,0,0,0};

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

  Wire.begin();
  
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //return;
  }
  Serial.println("card initialized.");
  if(SD.exists("move.txt")){
  SD.remove("move.txt");
  }
  mos(SHOOT,LOW);
  start_sound();
  Serial.println("HI");

}

void loop() {
  //SDstring = "";
  
  Radiolink();
  if(ch[5] < 50){
    sdwrite(String(angle),String(Z),String(r),String(maxspeed));
  }
  mot_go(angle,Z,r);
  if(ch[6] < 50){
      sdread();
  }
//  if(ch[10] < 50){
//    mos(PR1,HIGH);
//    mos(PR2,LOW);
//    if(ch[9]<50){
//      Shoot(HIGH);
//    }
//  }
//  else if (ch[10] > 200){
//    mos(PR1,LOW);
//    mos(PR2,HIGH);
//    if(ch[9]<50){
//      Shoot(LOW);
//    }
//  }
//  else{
//    mos(PR1,LOW);
//    mos(PR2,LOW);
//  }
//  
  
//  Serial.print(angle);
//  Serial.print("\t");
//  Serial.print(Z);
//  Serial.print("\t");
//  Serial.print(r);
//  Serial.print("\t");
//  Serial.println(maxspeed);
}
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
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
void Shoot(bool st){while(1){
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
  delay(500);
  mos(SHOOT,HIGH);
  delay(1000);
  mos(LED,LOW);
  mos(SHOOT,LOW);
  if(st==HIGH){mos(LIFT,LOW);}
  else{mos(LIFT,LOW);}
  break;
}
}
void mot_go(float x,float y,float r){
  fangle=x+180;
  serAngle=fangle*255/360;
  serZ=y;
  serr=r;
  if(serr > 255){serr=255;}
  if(serr <0){serr=0;}
  
  tell_uno(serAngle,serZ,serr);
}
/*/------------------------LCD------------------------------*/
void disp(){
  LCD.clrScr();
  LCD.setFont   (SmallFont);
  LCD.print     ("dist: ",LEFT,   0);
  //LCD.print     ("Speed: ",LEFT,   24);
  LCD.setFont   (MediumNumbers);
  //LCD.printNumI (Z,   CENTER, 24);
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
  if(x>0){
    x=x*x/255;
  }
  else{
    x=-x*x/255;
  }
  if(y>0){
    y=y*y/255;
  }
  else{
    y=-y*y/255;
  }
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
