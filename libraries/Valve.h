#ifndef Valve_h
#define Valve_h

#include "Arduino.h"

class Valve{
  public:
    Valve(int pinPos, int pinNeg);
    int motorPosition;
    int lastChange;
    int changeTime;
  private:
    int _pinPos;
    int _pinNeg;
};

#endif
