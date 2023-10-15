/*
   MUST-PES ABU ROBOCON 2018
   Program by: Shijir

   Master Mega

*/
#include <LCD5110_Basic.h>
#include <PS2X_lib.h>
#include <math.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
/*------------------------SD------------------------*/
const int chipSelect = 53;
File dataFile;
int sddel=13;
//int l,g =0;
/*------------------------Input Output Pins------------------------*/
int digitalIO[8] = {23, 25, 27, 29, 31, 33, 35, 37};
int analogpin[16] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15};
/*----------DIGITAL IO----*/
#define PULLSW 0
#define FRONTSENS1 1
#define FRONTSENS2 2
#define BALLSENSOR 3
#define INDIC1 47
#define INDIC2 49
/*-------------MOSFET----*/
#define PULLMOTOR 1
#define PUSH 3
#define CLUTCH 4
#define CLAMP 6
#define LENGH 7
byte mosdata = B00000000;
/////////////////////////////////0 1 shatsan
/*--------ANALOG------*/
#define FRONTSHARP 0
#define LEFTSHARP 1
/*--------------------------SHARP----------------------------*/
/*---------------------------LIDAR-------------------------*/

/*------------------------Serial Variable------------------------*/
byte serAngle, serZ;
double angle, Z;
double fangle;
double x, y;
double oldx, oldy;
double r;
byte serr;
byte maxspeed = 100;
byte ch[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*-----------------------------PS2----------------------------*/
PS2X ps2x;
int error = 0;
byte vibrate = 0;
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
int i = 0;
int buzz = 8;
int beeph = 100;
/*------------------------Motor------------------------*/
int motorrpm = 333;

/*----------------------------------------------------*/
int pushcount = 0;
bool pullst = LOW;
bool pushst = LOW;
void setup() {

  for (int i = 0; i < 8; i++) {
    pinMode(digitalIO[i], INPUT_PULLUP);
  }
  for (int i = 0; i < 16; i++) {
    pinMode(analogpin[i], INPUT);
  }
  pinMode(INDIC1,OUTPUT);
  pinMode(INDIC2,OUTPUT);
  pinMode(buzz, OUTPUT);
  Serial.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  //PS2();
  LCD.InitLCD();
  LCD.setFont(SmallFont);
  //error = ps2x.config_gamepad(9, 11, 10, 12, true, true); //(clock-9, command-11, attention-10, data-12)

  Wire.begin();

  //  Serial.print("Initializing SD card...");
  //  if (!SD.begin(chipSelect)) {
  //    Serial.println("Card failed, or not present");
  //    return;
  //  }
  //  Serial.println("card initialized.");
  //  SD.remove("angle.txt");
  //  SD.remove("z.txt");
  //  SD.remove("rot.txt");
  //  SD.remove("spd.txt");
  //
  start_sound();

}

void loop() {
  
  Radiolink();
  mot_go(angle, Z, r,maxspeed);
  pull();
  target();
  push();
  clamp();
  lenghten();
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------*/
void target() {
  if(digitalRead(digitalIO[FRONTSENS1]) == LOW){digitalWrite(INDIC1,HIGH);}
  else{digitalWrite(INDIC1,LOW);}
  if(digitalRead(digitalIO[FRONTSENS2]) == LOW){digitalWrite(INDIC2,HIGH);}
  else{digitalWrite(INDIC2,LOW);}
  //int l=0;
// if (digitalRead(digitalIO[FRONTSENS1]) == LOW){l++;}
// if (l > 1){
//    if (digitalRead(digitalIO[FRONTSENS1]) == HIGH){
//        for (int q=0; q <5 ; q++){
//    digitalWrite(INDIC1,HIGH);
//    delay(100);
//    digitalWrite(INDIC1,LOW);
//    delay(100);
//    }
//    }
//    
//  else if (digitalRead(digitalIO[FRONTSENS1]) == LOW) {digitalWrite(INDIC1,HIGH);}
// }
 /* if(digitalRead(digitalIO[FRONTSENS2]) == LOW){
    if (digitalRead(digitalIO[FRONTSENS2]) == HIGH){
      digitalWrite(INDIC2,HIGH);
      }
    else{digitalWrite(INDIC2,LOW);}
  }
  else if (digitalRead(digitalIO[FRONTSENS1]) == LOW && digitalRead(digitalIO[FRONTSENS2]) == LOW ){
    digitalWrite(INDIC1,HIGH);
    digitalWrite(INDIC2,HIGH);
  else {
    digitalWrite(INDIC1,LOW);
    digitalWrite(INDIC2,LOW);
  }
  }*/
}
bool PCst;
void pull() {
  if (ch[7] < 50) {
    if (pullst == LOW) {
      if (digitalRead(digitalIO[PULLSW]) == HIGH) {
        mos(PULLMOTOR, HIGH);
        mos(CLUTCH, HIGH);
      }
      else {
        mos(PULLMOTOR, LOW);
        mos(CLUTCH, LOW);
        pullst = HIGH;
      }
    }
  }
  else if(ch[8] > 50 || PCst == true){
    
      if (digitalRead(digitalIO[PULLSW]) == HIGH) {
        mos(PULLMOTOR, HIGH);
        mos(CLUTCH, HIGH);
        PCst = true;}
      else {
        mos(PULLMOTOR, LOW);
        mos(CLUTCH, LOW);
        PCst = false;}
  }
  else {
    mos(PULLMOTOR, LOW);
    mos(CLUTCH, LOW);
    pullst = LOW;
  }
}
//bool clamppst = true;
bool clampst = false;
void clamp(){
  if(ch[8]<50){
    if(digitalRead(digitalIO[BALLSENSOR]) == LOW && clampst == false){
      mos(CLAMP,LOW);
      clampst = true;
    }
  }
  else if(ch[8] > 50){
    mos(CLAMP,HIGH);
    clampst = false;
  }
//  else if (ch[5]<50 && clampst == false ){
//    if (clamppst == true){
//    mos(CLAMP,LOW);
//  //  clamppst = false;
//  }
//    else {
//    //  clamppst == true;
//      mos(CLAMP,HIGH);
//      }
//  }
}
//void clamp() {
//  if (ch[8] < 50 ) {
//   if (digitalRead(digitalIO[FRONTSENS1]) == LOW){l++; g=0;}
//   if (digitalRead(digitalIO[FRONTSENS2]) == LOW){g++; l=0;}
//   else {mos(CLAMP, HIGH);}
//   }
//  if (l >= 1){ mos(CLAMP, LOW);}
//  if (g >= 1){ mos(CLAMP, LOW);}
//  if (ch[8] > 50){mos(CLAMP, HIGH);}
//  //else {}
//}
//
void push () {
  if (ch[9] < 50) {
    mos(PUSH, HIGH);
  }
  else {
    mos(PUSH, LOW);
  }
}
void lenghten () {
  if (ch[10] < 50){
    mos(LENGH, HIGH);
  }
  else {mos(LENGH, LOW);}
}

void mos(int x, bool y) {
  byte highdata = B10000000 >> x;;
  byte lowdata = ~highdata;
  if (y == HIGH) {
    mosdata |= highdata;
  }
  else {
    mosdata &= lowdata;
  }
  Wire.beginTransmission(10);
  Wire.write(mosdata);
  Wire.endTransmission();
}

void mot_go(float x, float y, float r, float zspd) {
  fangle = x + 180;
  serAngle = fangle * 255 / 360;
  serZ = y;
  serr = r;
  if (serr > 255) {
    serr = 255;
  }
  if (serr < 0) {
    serr = 0;
  }
  maxspeed=zspd;
  tell_uno(serAngle, serZ, serr);
}
/*------------------------LCD------------------------------*/
void disp() {
  LCD.clrScr();
  LCD.setFont   (SmallFont);
  LCD.print     ("dist: ", LEFT,   0);
  //LCD.print     ("Speed: ",LEFT,   24);
  LCD.setFont   (MediumNumbers);
  //LCD.printNumI (Z,   CENTER, 24);
}
/*----------------------Serial Receive-------------------------*/
void Radiolink() {
  Wire.requestFrom(9, 11);
  while (Wire.available()) {
    for (int i = 0; i < 11; i++) {
      ch[i] = Wire.read();
      if (ch[i] < 130 && ch[i] > 123) {
        ch[i] = 127;
      }
    }
  }
  x = (ch[3] - 127) * 2;
  y = (ch[1] - 127) * 2;
  r = ch[0];
  if (x > -40 && x < 40) {
    x = 0;
  }
  if (y > -40 && y < 40) {
    y = 0;
  }
  if (x > 255) {
    x = 255;
  }
  if (x < -255) {
    x = -255;
  }
  if (y > 255) {
    y = 255;
  }
  if (y < -255) {
    y = -255;
  }
  angle = atan2(y, x) * 180 / PI;
  Z = pow(pow(y, 2) + pow(x, 2), 0.5);
  if (Z > 255) {
    Z = 255;
  }
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
void start_sound() {
  for (i = 0; i < 1000; i++) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz, LOW);
    delayMicroseconds(50);
  }
  delay(20);
  for (i = 0; i < 1000; i++) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz, LOW);
    delayMicroseconds(40);
  }
  delay(20);
  for (i = 0; i < 1000; i++) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz, LOW);
    delayMicroseconds(30);
  }
  delay(20);
  for (i = 0; i < 1000; i++) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz, LOW);
    delayMicroseconds(20);
  }
  delay(20);
  for (i = 0; i < 1500; i++) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz, LOW);
    delayMicroseconds(10);
  }
  delay(1000);
  for (i = 0; i < 1500; i++) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz, LOW);
    delayMicroseconds(30);
  }
}
