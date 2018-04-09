#include "Arduino.h"
#include "Valve.h"

Valve::Valve(Gauge g){
  _pinPos = 0;
  _pinNeg = 0;
  gauge = g;
  lastChange = millis();
  lastInput = millis();
  lastColorChange = millis();
  changeTime = random(1, 3);
  changeColorTime = random(5, 8);
  inputDelay = 1000;
  posCap = 255;
  negCap = 0;
  acceptablePercentage = 6;
  range = random(0, 5);
}

Valve::Valve(int pinPos, int pinNeg){
  _pinPos = pinPos;
  _pinNeg = pinNeg;
  pinMode(pinPos, INPUT);
  pinMode(pinNeg, INPUT);
  lastChange = millis();
  changeTime = random(30, 121);
}

Valve::Valve(int pinPos, int pinNeg, Gauge g){
  _pinPos = pinPos;
  _pinNeg = pinNeg;
  pinMode(pinPos, INPUT);
  pinMode(pinNeg, INPUT);
  gauge = g;
  lastChange = millis();
  lastInput = millis();
  changeTime = random(1, 6);
  inputDelay = 1000;
  vpCounter = 0;
  vnCounter = 0;
  posCap = 253;
  negCap = 3;
  acceptablePercentage = 6;
  arrSize = (sizeof(vpValues) / sizeof(*vpValues));
}

void Valve::valveInput(int vp, int vn){
  vpValues[vpCounter] = vp;
  vnValues[vnCounter] = vn;

  if(vpCounter < 9 && vpCounter >= 0){
    vpCounter++;
  }
  else{
    vpCounter = 0;
  }

  if(vnCounter < 9 && vnCounter >= 0){
    vnCounter++;
  }
  else{
    vnCounter = 0;
  }
  
  int vpPercentage = 0;
  int vnPercentage = 0;
  
  for(int x = 0; x < arrSize; x++){
    if(vpValues[x] == 1){
      vpPercentage++;
    }
    if(vnValues[x] == 1){
      vnPercentage++;
    }
  }
  
  if(vpPercentage >= acceptablePercentage && (millis() - lastInput > inputDelay)){
    if(gauge.gaugePosition < posCap){
      gauge.gaugePosition += 25;
      analogWrite(gauge.gaugePin0, gauge.gaugePosition);
      Serial.println("Positive Change");
      Serial.print("Gauge Position: ");
      Serial.println(gauge.gaugePosition);
      lastInput = millis();
    }
  }

  if(vnPercentage >= acceptablePercentage && (millis() - lastInput > inputDelay)){
    if(gauge.gaugePosition > negCap){
      gauge.gaugePosition -= 25;
      analogWrite(gauge.gaugePin0, gauge.gaugePosition);
      Serial.println("Negative Change");
      Serial.print("Gauge Position: ");
      Serial.println(gauge.gaugePosition);
      lastInput = millis();
    }
  }
}

void Valve::randomChange() {
  int directionChange;
  
  if (millis() - lastChange >= (changeTime * 1000)) {
    directionChange = random(-1, 2);
    //Serial.print("Direction Change: ");
    //Serial.println(directionChange);
    if (gauge.gaugePosition <= posCap && directionChange > 0){
      gauge.gaugePosition += (directionChange * 25);
      if (gauge.gaugePosition <= posCap){
      	analogWrite(gauge.gaugePin0, gauge.gaugePosition);
      }
      else{
		    gauge.gaugePosition -= (directionChange * 25);
      }
    }
    if (gauge.gaugePosition >= negCap && directionChange < 0){
      gauge.gaugePosition += (directionChange * 25);
      if (gauge.gaugePosition >= negCap){
      	analogWrite(gauge.gaugePin0, gauge.gaugePosition);
      }
      else{
		    gauge.gaugePosition -= (directionChange * 25);
      }
    }
    lastChange = millis();
    changeTime = random(1, 3);
  }
}

void Valve::randomColor(){
  if(millis() - lastColorChange >= (changeColorTime * 1000)){
    range = random(0, 5);
    lastColorChange = millis();
    changeColorTime = (5, 8);
  }
}

Gauge::Gauge(){
  
}

Gauge::Gauge(int pin0){
  gaugePin0 = pin0;
  pinMode(gaugePin0, OUTPUT);
  gaugePosition = 128;
}

Gauge::Gauge(int pin0, int pin1, int pin2, int pin3){
  gaugePin0 = pin0;
  gaugePin1 = pin1;
  gaugePin2 = pin2;
  gaugePin3 = pin3;
  pinMode(gaugePin0, OUTPUT);
  pinMode(gaugePin1, OUTPUT);
  pinMode(gaugePin2, OUTPUT);
  pinMode(gaugePin3, OUTPUT);
  gaugePosition = 0;
}