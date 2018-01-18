#include "Arduino.h"
#include "Valve.h"

Valve::Valve(int pinPos, int pinNeg){
  _pinPos = pinPos;
  _pinNeg = pinNeg;
  pinMode(pinPos, INPUT);
  pinMode(pinNeg, INPUT);
  motorPosition = 0;
  lastChange = millis();
  changeTime = random(30, 121);
}

