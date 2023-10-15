
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
int M1inA=7, M1inB=8;
int M2inA=4, M2inB=9;
int M1pwm=5, M2pwm=6;
int M1en=A0, M2en=A1;

int normalsw;
int nintysw;
#define FORWARD 1
#define BACKWARD 2
/*--------------------------Serial Variable--------------------------*/
byte dir=0; 
byte spd=0;

/*------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------SETUP-------------------------------*/
void setup() {
  Wire.begin(11);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  pinMode(M1inA,OUTPUT); pinMode(M1inB,OUTPUT);
  pinMode(M2inA,OUTPUT); pinMode(M2inB,OUTPUT);
  pinMode(M1pwm,OUTPUT); pinMode(M2pwm,OUTPUT);
  pinMode(M1en,OUTPUT); pinMode(M2en,OUTPUT);
  pinMode(13,INPUT_PULLUP);pinMode(11,INPUT_PULLUP);
  digitalWrite(M1en,HIGH);
  digitalWrite(M2en,HIGH);
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
  nintysw = digitalRead(13);
  normalsw = digitalRead(11);
  Serial.print(nintysw);
  Serial.print("  ");
  Serial.println(normalsw);
  if(dir == FORWARD){
    if(nintysw == HIGH){
      mot(dir,spd);
    }
    else{
      mot(dir,30);
    }
  }
  else if(dir == BACKWARD){
    if(normalsw == HIGH){
      mot(dir,spd);
    }
    else{
      mot(dir,30);
    }
  }
  else{
    mot(3,255);
  }
}
/*--------------------------Motor Control-------------------------*/
void mot(int x,int y){
  digitalWrite(M1en,HIGH);
  digitalWrite(M2en,HIGH);
  if(x == 1){
    digitalWrite(M1inA,HIGH);
    digitalWrite(M1inB,LOW);
    digitalWrite(M2inA,LOW);
    digitalWrite(M2inB,HIGH);
    analogWrite(M1pwm,abs(y));
    analogWrite(M2pwm,abs(y));
  }
  else if(x == 2){
    digitalWrite(M1inA,LOW);
    digitalWrite(M1inB,HIGH);
    digitalWrite(M2inA,HIGH);
    digitalWrite(M2inB,LOW);
    analogWrite(M1pwm,abs(y));
    analogWrite(M2pwm,abs(y));
  }
  else{
    digitalWrite(M1inA,LOW);
    digitalWrite(M1inB,LOW);
    digitalWrite(M2inA,LOW);
    digitalWrite(M2inB,LOW);
    analogWrite(M1pwm,255);
    analogWrite(M2pwm,255);
  }
}

/*--------------------------Serial Input--------------------------*/
void receiveEvent(int howMany) {
  while (1 < Wire.available()) {
    dir = Wire.read();
    spd = Wire.read();  
  }
//  Serial.print(serAngle);
//  Serial.print("\t");
//  Serial.print(serZ);
//  Serial.print("\t");
//  Serial.print(serr);
//  Serial.print("\t");
//  Serial.println(maxspeed);
}
void requestEvent(){
  Wire.write(nintysw);
  Wire.write(normalsw);
}
