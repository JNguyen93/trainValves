#include <Valve.h>

const byte motorPin1 = 2;
const byte motorPin2 = 3;
const byte motorPin3 = 4;
const byte motorPin4 = 5;
const byte valve0p = A0;
const byte valve0n = A1;
const byte valve1p = A2;
const byte valve1n = A3;
const byte valve2p = A4;
const byte valve2n = A5;
const byte greenLED = 9;
const byte redLED = 10;
const byte relay = 8;
boolean v0p = false;
boolean v0n = false;
boolean v1p = false;
boolean v1n = false;
boolean v2p = false;
boolean v2n = false;
boolean correctVial = false;
int motorSpeed = 1200;
int count = 0;
int countsperrev = 90;
int lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};
int lastChange;
int motorPosition;
int changeTime;
int directionChange;

void setup() {
  Serial.begin(9600);
  Serial.print("Valve System Online");
  pinMode(relay, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  Valve valve0(valve0p, valve0n);
  Valve valve1(valve1p, valve1n);
  Valve valve2(valve2p, valve2n);
  lastChange = millis();
 }

void loop() {
  changeTime = random(30, 121);
  
  if(millis()- lastChange >= changeTime){
    directionChange = random(-1, 2);
    
    if(directionChange == 1){
      clockwise();
    }
    else if(directionChange == -1){
      counterclockwise();
    }
    
    motorPosition += directionChange;
    
    if(motorPosition == 0){
      digitalWrite(relay, HIGH);
    }
    else{
      digitalWrite(relay, LOW);
    }
  }
  
}

void counterclockwise(){
  for (int i = 0; i < 8; i++){
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

void clockwise(){
  for (int i = 7; i >= 0; i--){
    setOutput(i);
    delayMicroseconds(motorSpeed);
  }
}

void setOutput(int out){
  digitalWrite(motorPin1, bitRead(lookup[out], 0));
  digitalWrite(motorPin2, bitRead(lookup[out], 1));
  digitalWrite(motorPin3, bitRead(lookup[out], 2));
  digitalWrite(motorPin4, bitRead(lookup[out], 3));
}


