#define ENCODER_DO_NOT_USE_INTERRUPTS

#include <Valve.h>
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Bounce2.h>

//PWM Pins: 3,5,6,9,10,11
//Interrupt Pins: 2,3
//SCL: A5
//SDA: A4
//Encoder Pins Used: 2,3,4,5
const int resetButton = 6;
const int relay = 7;
const int gauge0pin = 10; //Left Gauge
const int gauge1pin = 9; //11 -> 9 Right Gauge
const int buttonLED = 8;
const int red0 = 13;
const int green0 = 12;
const int blue0 = 11;
const int red1 = A2;
const int green1 = A1;
const int blue1 = A0;
int leftLED[3]= {red0, green0, blue0};
int rightLED[3] = {red1, green1, blue1};
int* LEDS[2] = {leftLED, rightLED};
int barCounter0 = 23;
int barCounter1 = 23;
int enCounter0 = 0;
int enCounter1 = 0;
int solCounter0 = 0;
int solCounter1 = 0;
int winNumber = 15;
bool colorChange0;
bool colorChange1;
bool flag0;
long long flag0Set;
bool flag1;
long long flag1Set;
int penaltyDelay = 3500;
bool penalty0 = false;
bool penalty1 = false;
bool win = false;
bool win0 = false;
bool win1 = false;
bool error0;
bool error1;
long long errorSet;
bool errorOn = false;
bool added0 = false;
bool added1 = false;

int off[3] = {0, 0, 0};
int red[3] = {255, 0 , 0};
int green[3] = {0, 255, 0};
int blue[3] = {0, 0, 255};
int yellow[3] = {255, 255, 0};
int purple[3] = {255, 0, 255};
int aqua[3] = {0, 255, 255};
int test[3] = {255, 255, 255};
int* colors[5] = {purple, red, blue, yellow, aqua};

Encoder encoder0(2, 4);
Gauge gauge0 = Gauge(gauge0pin);
Valve valve0 = Valve(gauge0);
Encoder encoder1(3, 5);
Gauge gauge1 = Gauge(gauge1pin);
Valve valve1 = Valve(gauge1);
Adafruit_24bargraph bar0 = Adafruit_24bargraph();
Adafruit_24bargraph bar1 = Adafruit_24bargraph();

Bounce reset = Bounce();

void setup() {
  Serial.begin(9600);
  Serial.println("Valve System Online");
  //bar0.begin(0x70);
  //bar1.begin(0x71);
  pinMode(relay, OUTPUT);
  pinMode(buttonLED, OUTPUT);
  pinMode(resetButton, INPUT_PULLUP);
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 3; j++){
      pinMode(LEDS[i][j], OUTPUT);
    }
  }
  digitalWrite(relay, HIGH);
  digitalWrite(buttonLED, LOW);
  analogWrite(gauge0pin, 128);
  reset.attach(resetButton);
  reset.interval(50);

  /*for (uint8_t b = 0; b < 24; b++){
    if (b >= 0 && b < 8)  bar0.setBar(b, LED_RED);
    if (b >= 8 && b < 16)  bar0.setBar(b, LED_YELLOW);
    if (b >= 16 && b < 24)  bar0.setBar(b, LED_GREEN);
    bar0.writeDisplay();
    delay(50);
  }*/
}

void loop() {
  /*Serial.println("Gauge Position: ");
  Serial.println(valve0.gauge.getGauge());
  Serial.println("Time Until Next Change: ");
  Serial.println(valve0.getChangeTime());*/

  //barFlicker(bar0, barCounter0);
  //Serial.println("Initiating Valve Change");
  if(!win0){
    valve0.randomChange();
    colorChange0 = valve0.randomColor();
    readEncoder(encoder0, valve0, enCounter0);
    setColor(leftLED, colors[valve0.getRange()]);
    if(colorChange0){
      added0 = false;
    }
    if (valve0.gauge.gaugePosition >= valve0.getRangeMin() && valve0.gauge.gaugePosition <= valve0.getRangeMax()) {
      flag0 = false;
      if(!added0){
        solCounter0++;
        added0 = true;
      }
    }
    else {
      if(!flag0){
        flag0Set = millis();
      }
      flag0 = true;
    }
    penaltyCheck(flag0, flag0Set, penalty0, solCounter0);
    if(solCounter0 >= winNumber){
      win0 = true;
      solCounter0 = 0;
    }
  }
  else{
    setColor(leftLED, green);
  }
  
  if(!win1){
    valve1.randomChange();
    colorChange1 = valve1.randomColor();
    readEncoder(encoder1, valve1, enCounter1);
    setColor(rightLED, colors[valve1.getRange()]);
    if(colorChange1){
      added1 = false;
    }
    if (valve1.gauge.gaugePosition >= valve1.getRangeMin() && valve1.gauge.gaugePosition <= valve1.getRangeMax()) {
      flag1 = false;
      if(!added1){
        solCounter1++;
        added1 = true;
      }
    }
    else {
      if(!flag1){
        flag1Set = millis();
      }
      flag1 = true;
    }
    penaltyCheck(flag1, flag1Set, penalty1, solCounter1);
    if(solCounter1 >= winNumber){
      win1 = true;
      solCounter1 = 0;
    }
  }
  else{
    setColor(rightLED, green);
  }

  if(win0 && win1){
    setColor(leftLED, green);
    setColor(rightLED, green);
    digitalWrite(relay, HIGH);
    digitalWrite(buttonLED, LOW);
    delay(600000);
    win0 = false;
    win1 = false;
  }
  else{
    if(penalty0 && penalty1){
      shutdownPanel();
    }
  }
}

void readEncoder(Encoder& e, Valve& v, int& counter) {
  signed long eValue = e.read();
  //Serial.println(eValue);
  if(eValue > 0){
    counter++;
  }
  else if(eValue < 0){
    counter--;
  }
  //Serial.println(counter);
  if(counter >= 20){
    v.gauge.gaugePosition++;
    counter = 0;
  }
  if(counter <= -20){
    v.gauge.gaugePosition--;
    counter = 0;
  }
  if(v.gauge.gaugePosition >= 255){
    v.gauge.gaugePosition = 255;
    e.write(0);
    //Serial.println("Overflow: Reset");
  }
  else if(v.gauge.gaugePosition <= 0){
    v.gauge.gaugePosition = 0;
    e.write(0);
    //Serial.println("Underflow: Reset");
  }
  analogWrite(v.gauge.gaugePin0, v.gauge.gaugePosition);
  e.write(0);
  //Serial.print("Gauge Position: ");
  //Serial.println(v.gauge.gaugePosition);
}

void barFlicker(Adafruit_24bargraph& bar, int& counter){
  if(counter >= 16 && counter < 24){
    bar.setBar(counter, LED_OFF);
    bar.writeDisplay();
    counter--;
    delay(100);
    bar.setBar(counter, LED_GREEN);
    bar.writeDisplay();
    counter++;
  }
  else if(counter >= 8 && counter < 16){
    bar.setBar(counter, LED_OFF);
    bar.writeDisplay();
    counter--;
    delay(100);
    bar.setBar(counter, LED_YELLOW);
    bar.writeDisplay();
    counter++;
  }
  else if(counter >= 0 && counter < 8){
    bar.setBar(counter, LED_OFF);
    bar.writeDisplay();
    counter--;
    delay(100);
    bar.setBar(counter, LED_RED);
    bar.writeDisplay();
    counter++;
  }
  else{
    counter = 0;
  }
}

void penaltyCheck(bool& flag, long long& flagSet, bool& penalty, int& solCounter){
  if(millis() - flagSet >= penaltyDelay && flag){
    penalty = true;
    solCounter = 0;
  }
  else{
    //penalty = false;
  }
}

void penaltyCheck(Adafruit_24bargraph& bar, int& counter, bool& flag, long long& flagSet, bool& penalty){
  if(millis() - flagSet >= penaltyDelay && flag){
    for(int i = 0; i < 8; i++){
      bar.setBar(counter, LED_OFF);
      bar.writeDisplay();
      counter--;
    }
    penalty = true;
  }
  else if (penalty && !flag){
    if(counter >= 16 && counter < 24){
      for(int i = 0; i < 8; i++){
        bar.setBar(counter, LED_GREEN);
        bar.writeDisplay();
        counter++;
      }
    }
    else if(counter >= 8 && counter < 16){
      for(int i = 0; i < 8; i++){
        bar.setBar(counter, LED_YELLOW);
        bar.writeDisplay();
        counter++;
      }
    }
    else if(counter >= 0 && counter < 8){
      for(int i = 0; i < 8; i++){
        bar.setBar(counter, LED_RED);
        bar.writeDisplay();
        counter++;
      }
    }
    penalty = false;
  }
}

void initializeBar(Adafruit_24bargraph& bar, int& counter){
  for (uint8_t b = 0; b < 24; b++){
    if (b >= 0 && b < 8)  bar.setBar(b, LED_RED);
    if (b >= 8 && b < 16)  bar.setBar(b, LED_YELLOW);
    if (b >= 16 && b < 24)  bar.setBar(b, LED_GREEN);
    bar.writeDisplay();
    delay(50);
  }
  counter = 23;
}
void shutdownPanel(){
  /*for(int i = 0; i < barCounter0 + 1; i++){
    bar0.setBar(barCounter0, LED_OFF);
    bar0.writeDisplay();
    barCounter0--;
  }*/
  /*for(int i = 0; i < barCounter1 + 1; i++){
    bar1.setBar(barCounter1, LED_OFF);
    bar1.writeDisplay();
    barCounter1--;
  }*/
  reset.update();
  digitalWrite(relay, LOW);
  digitalWrite(buttonLED, HIGH);
  if(reset.fell()){
    //Serial.println("Resetting...");
    digitalWrite(buttonLED, LOW);
    digitalWrite(relay, HIGH);
    //initializeBar(bar0, barCounter0);
    //initializeBar(bar1, barCounter1);
    flag0 = false;
    flag1 = false;
    penalty0 = false;
    penalty1 = false;
  }
}

void setColor(int* led, int* color){
  #ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  #endif
  /*Serial.print("Red: ");
  Serial.print(color[0]);
  Serial.print(" Green: ");
  Serial.print(color[1]);
  Serial.print(" Blue: ");
  Serial.println(color[2]);*/
  digitalWrite(led[0], color[0]);
  digitalWrite(led[1], color[1]);
  digitalWrite(led[2], color[2]);
}
