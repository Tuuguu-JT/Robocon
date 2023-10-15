
/*
 * MUST-PES ABU ROBOCON 2018
 * Program by: Shijir
 * 
 * Slave Uno #1
 * 
*/

#include<math.h>
#include <PID_v1.h>
#include "PinChangeInterrupt.h"
#include<Wire.h>

/*--------------------------Motor Pins--------------------------*/
int M1inA=2, M1inB=4;
int M2inA=7, M2inB=8;
int M1pwm=9, M2pwm=10;
int M1en=6, M2en=12;
/*-------------------------- Motor Variable --------------------------*/
int motorRPM=333;

/*--------------------------Serial Variable--------------------------*/
byte data[8]; 
byte  serAngle, serZ, serr;
byte maxspeed;
int  a;

/*--------------------------Interrupt Variable--------------------------*/
boolean intrupcon1=LOW,intrupcon2=LOW;
long timea1=0,timeb1=0, timea2=0,timeb2=0 ;
long rpm1=0, rpm2=0;

/*-------------------------- Omni Variable --------------------------*/
double r=0;
double spdA=0,spdB=0;
double angle,Z,fangle;

/*------------------------------- PID --------------------------------*/
double motKp=0.6375, motKi=15, motKd=0.0001;
double mot1Setpoint, mot1Input, mot1Output;
double mot2Setpoint, mot2Input, mot2Output;

PID mot1pid(&mot1Input, &mot1Output, &mot1Setpoint, motKp, motKi, motKd, DIRECT); 
PID mot2pid(&mot2Input, &mot2Output, &mot2Setpoint, motKp, motKi, motKd, DIRECT);  

/*---------------------------- Old Variable ---------------------------*/
int led0 = 10;
int led1 = 11;
int led2 = 12;
int led3 = 13;
int buzz = A4;
int btn  = A5;
int i;
int beeph = 100;


/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------SETUP-------------------------------*/
void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  
  Serial.begin(9600);
  pinMode(M1inA,OUTPUT); pinMode(M1inB,OUTPUT);
  pinMode(M2inA,OUTPUT); pinMode(M2inB,OUTPUT);
  pinMode(M1pwm,OUTPUT); pinMode(M2pwm,OUTPUT);
  pinMode(M1en,OUTPUT); pinMode(M2en,OUTPUT);
  digitalWrite(M1en,HIGH);
  digitalWrite(M2en,HIGH);
  
  pinMode(3,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);

  attachInterrupt(1,intrup1,RISING);                    ///Motor 1 PID
  attachPCINT(digitalPinToPCINT(5),intrup2,RISING);     ///Motor 2 PID
  
  mot1pid.SetMode(AUTOMATIC);
  mot2pid.SetMode(AUTOMATIC);
  ///////////////////////////////Old Setup/////////////////////////////
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(btn, INPUT);
  ////////////////////////////////////////////////////////////////////
}
/*-------------------------- Interrupt Func --------------------------*/
void intrup1(){
  if(intrupcon1 == LOW){
    timea1=micros();
    intrupcon1 = HIGH;
  }
  else{
    timeb1=micros()-timea1;
    intrupcon1 = LOW;
  }
}
/*--------------------------------------------------------------------*/
void intrup2(){
  if(intrupcon2 == LOW){
    timea2=micros();
    intrupcon2 = HIGH;
  }
  else{
    timeb2=micros()-timea2;
    intrupcon2 = LOW;
  }
}

/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*----------------------------Main Loop------------------------------*/
void loop(){
  moveCalc();
  //pidCalc();
  mot_1(spdA);
  mot_2(spdB);
}
/*--------------------------Move Calculation-------------------------*/
void moveCalc(){
  fangle=serAngle;
  angle=fangle/255*360-180;
  Z=serZ;
  r =(serr - 127)*2;
  if(r>-50&&r<50){r=0;}
  if(r>255){r=255;}
  if(r<-255){r=-255;}
  
  spdA=(Z*sin((45-angle)*PI/180)+r)*maxspeed/255;
  spdB=(Z*sin((315-angle)*PI/180)+r)*maxspeed/255;
  if(spdA > 255){spdA=255;}
  if(spdA < -255){spdA=-255;}
  if(spdB > 255){spdB=255;}
  if(spdB < -255){spdB=-255;}
}
/*--------------------------PID Calculation-------------------------*/
void pidCalc(){
  mot1Setpoint=abs(spdA)*motorRPM/255;
  mot2Setpoint=abs(spdB)*motorRPM/255;

  rpm1=60000000/(19*timeb1)/12;
  rpm2=60000000/(19*timeb2)/12;

  mot1Input=rpm1;
  mot2Input=rpm2;

  mot1pid.Compute();
  mot2pid.Compute();
}

/*--------------------------Motor Control-------------------------*/
void mot_1(int spd){
  digitalWrite(M1en,HIGH);
  if(spdA > 0){
    digitalWrite(M1inA,LOW);
    digitalWrite(M1inB,HIGH);
    analogWrite(M1pwm,abs(spd));
  }
  else if(spdA < 0){
    digitalWrite(M1inA,HIGH);
    digitalWrite(M1inB,LOW);
    analogWrite(M1pwm,abs(spd));
  }
  else{
    digitalWrite(M1inA,LOW);
    digitalWrite(M1inB,LOW);
    analogWrite(M1pwm,255);
  }
}
/*-----------------------------------------------------------------------*/
void mot_2(int spd){
  digitalWrite(M2en,HIGH);
  if(spdB > 0){
    digitalWrite(M2inA,LOW);
    digitalWrite(M2inB,HIGH);
    analogWrite(M2pwm,abs(spd));
  }
  else if(spdB < 0){
    digitalWrite(M2inA,HIGH);
    digitalWrite(M2inB,LOW);
    analogWrite(M2pwm,abs(spd));
  }
  else{
    digitalWrite(M2inA,LOW);
    digitalWrite(M2inB,LOW);
    analogWrite(M2pwm,255);
  }
}


/*--------------------------Serial Input--------------------------*/
void receiveEvent(int howMany) {
  while (1 < Wire.available()) {
    serAngle = Wire.read();
    serZ = Wire.read();  
    serr = Wire.read();
    maxspeed = Wire.read();   
  }
//  Serial.print(serAngle);
//  Serial.print("\t");
//  Serial.print(serZ);
//  Serial.print("\t");
//  Serial.print(serr);
//  Serial.print("\t");
//  Serial.println(maxspeed);
}

/*--------------------------Sound--------------------------*/
void start_sound(){
  delay(100);
  for(i=0;i<500;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(500);
  }
  delay(500);
  for(i=0;i<500;i++){
    digitalWrite(buzz,HIGH);
    delayMicroseconds(beeph);
    digitalWrite(buzz,LOW);
    delayMicroseconds(500);
  }

}
