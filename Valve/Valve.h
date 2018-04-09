#ifndef Valve_h
#define Valve_h

#include "Arduino.h"

class Gauge{
  public:
    Gauge();
    Gauge(int pin0);
    Gauge(int pin0, int pin1, int pin2, int pin3);
    int getGauge(){
	    return gaugePosition;
    }
    int gaugePin0;
    int gaugePin1;
    int gaugePin2;
    int gaugePin3;
    int gaugePosition;
};

class Valve{
  public:
    Valve(Gauge g);
    Valve(int pinPos, int pinNeg);
    Valve(int pinPos, int pinNeg, Gauge g);
    void valveInput(int vn, int vp);
    void randomChange();
    void randomColor();
    int getPinPos(){
      return _pinPos;
    }
    int getPinNeg(){
      return _pinNeg;
    }
    int getChangeTime(){
	    return changeTime;
    }
    int getRange(){
      return range;
    }
    int getRangeMin(){
      return ranges[range][0];
    }
    int getRangeMax(){
      return ranges[range][1];
    }
    Gauge gauge;
    boolean vpValues[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    boolean vnValues[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int vpCounter;
    int vnCounter;
    int posCap;
    int negCap;
    int acceptablePercentage;
    int arrSize;
    long long lastChange;
    long long lastInput;
    long long lastColorChange;
    int inputDelay;
    int changeTime;
    int changeColorTime;
    int range;
    int range0[2] = {0, 51};
    int range1[2] = {52, 102};
    int range2[2] = {103, 153};
    int range3[2] = {154, 204};
    int range4[2] = {205, 255};
    int* ranges[5] = {range0, range1, range2, range3, range4};
  private:
    int _pinPos;
    int _pinNeg;
};

#endif