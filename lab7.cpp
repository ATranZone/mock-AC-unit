/*        Your Name & E-mail: Andy Tran atran153@ucr.edu

*          Discussion Section: 023

 *         Assignment: Lab #7  

 *         Exercise Description: [optional - include for your own benefit]

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://youtu.be/EzWD8VRhvaQ

 */
#include <LiquidCrystal.h>
#include <DHT.h>
#define DHTPIN A0     
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int JS_BTN = 12;
int JS_X = A1;
int JS_Y = A2;
int blue = 8;
int yellow = 9;
int white = 10;
int red = 11;
int setTemp = 75;
int setHum = 30;
int systemPower = 0;
int countb = 0;
int countr = 0;
int county = 0;
int countw = 0;
typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 5;
task tasks[tasksNum];

enum SM1_States {tempOff, humOff, readInput, tempInc, tempDec, humMode, humInc, humDec};
int SM1_Tick(int state1) {
  int h = dht.readHumidity();
  float t = dht.readTemperature();
  int f = dht.readTemperature(true);
  
  switch (state1) { //state transitions
    case tempOff:
      if(digitalRead(JS_BTN) == 0){
      state1 = readInput;
      }
      break;
    case humOff:
      if(digitalRead(JS_BTN) == 0){
        state1 = humMode;
      }
      break;
    case readInput:
      if(digitalRead(JS_BTN) == 0){
        state1 = tempOff;
      }
      if(analogRead(JS_Y)>900){ //right
        state1 = tempInc;
      }
      if(analogRead(JS_Y)<100){ //left
        state1 = tempDec;
      }
      if(analogRead(JS_X)<100){//down
        state1 = humMode;
      }
      break;
    case tempInc:
      state1 = readInput;
      break;
    case tempDec:
      state1 = readInput;
      break;
    case humMode:
      if(digitalRead(JS_BTN) == 0){
        state1 = humOff;
      }
      if(analogRead(JS_Y)>900){ //right
        state1 = humInc;
      }
      if(analogRead(JS_Y)<100){ //left
        state1 = humDec;
      }
      if(analogRead(JS_X)>900){//down
        state1 = readInput;
      }
      break;
    case humInc:
      state1 = humMode;
      break;
    case humDec:
      state1 = humMode;
      break;
  }
  switch (state1) { //state actions
    case tempOff:
      lcd.noDisplay();
      systemPower = 0;
      break;
    case humOff:
      lcd.noDisplay();
      systemPower = 0;
      break;
    case readInput:
      systemPower = 1;
      lcd.display();
      lcd.setCursor(0,0);
      lcd.print("Temp:");
      lcd.print(f);
      lcd.print(" STemp:");
      lcd.print(setTemp);
      lcd.setCursor(0,1);
      lcd.print("Hum:");
      lcd.print(h);
      lcd.print(" SHum:");
      lcd.print(setHum); 
      break;
    case tempInc:
      setTemp++;
      break;
    case tempDec:
      setTemp--;
      break;
    case humMode:
      systemPower = 1;
      lcd.display();
      lcd.setCursor(0,0);
      lcd.print("Temp:");
      lcd.print(f);
      lcd.print(" STemp:");
      lcd.print(setTemp);
      lcd.setCursor(0,1);
      lcd.print("Hum:");
      lcd.print(h);
      lcd.print(" SHum:");
      lcd.print(setHum); 
      break;
    case humInc:
      setHum++;
      break;
    case humDec:
      setHum--;
      break;
  }
  return state1;
}

enum SM2_States {sm_init, off, on}; //blue
int SM2_Tick(int state2){

  int h = dht.readHumidity();
  
  switch (state2){ //state transitions
    case sm_init:
      state2 = off;
      break;
    case off:
      
      if(setHum > h && systemPower == 1){
        state2 = on;
      }
      if (setHum == h){
        state2 = off;
      }
      break;
    case on:
      if(setHum < h){
        state2 = off;
      }
      if(setHum == h){
        state2 = off;
      }
      if(systemPower == 0){
        state2 = off;
      }
      if(setHum > h && systemPower == 1){
        state2 = on;
      }
      break;
    
  }

  switch (state2){ //state actions
    
    case sm_init:   
      digitalWrite(blue,LOW);
      break;
    case off:
      digitalWrite(blue,LOW);
      break;
    case on:
      if(countb == 0){
        digitalWrite(blue,HIGH);
      }
      
      else{
        digitalWrite(blue,LOW);
      }
      countb++;
      if(countb==4){
        countb = 0;
      }
      break;
  }
  return state2;
}

enum SM3_States {sm3_init, off3, on3};
int SM3_Tick(int state3){
  //int count = 0;
  int h = dht.readHumidity();
  
  switch (state3){ //state transitions
    case sm3_init:
      state3 = off3;
      break;
    case off3:
      if(setHum < h && systemPower == 1){
        state3 = on3;
      }
      if (setHum == h){
        state3 = off3;
      }
      break;
    case on3:
      if(setHum > h){
        state3 = off3;
      }
      if(setHum == h){
        state3 = off3;
      }
      if(systemPower == 0){
        state3 = off3;
      }
      break;
  }

  switch (state3){ //state actions
    case sm3_init:
      digitalWrite(yellow,LOW);
      break;
    case off3:
      digitalWrite(yellow,LOW);
      break;
    case on3:
      if(county == 0){
        digitalWrite(yellow,HIGH);
      }
      
      else{
        digitalWrite(yellow,LOW);
      }
      county++;
      Serial.println(county);
      if(county == 3){
        county = 0;
      }
      break;
  }
  return state3;
}

enum SM4_States {sm4_init, off4, on4};
int SM4_Tick(int state4){
  //int count = 0;
  float t = dht.readTemperature();
  int f = dht.readTemperature(true);
  switch (state4){ //state transitions
    case sm4_init:
      state4 = off4;
      break;
    case off4:
      if(setTemp > f && systemPower == 1){
        state4 = on4;
      }
      if (setTemp == f){
        state4 = off4;
      }
      break;
    case on4:
      if(setTemp < f){
        state4 = off4;
      }
      if(setTemp == f){
        state4 = off4;
      }
      if(systemPower == 0){
        state4 = off4;
      }
      break;
  }

  switch (state4){ //state actions
    case sm4_init:
      digitalWrite(red,LOW);
      break;
    case off4:
      digitalWrite(red,LOW);
      break;
    case on4:
      if(countr!=18){
      digitalWrite(red,HIGH);
      }
      else{
        digitalWrite(red,LOW);
      }
      countr++;
      if(countr==19){
        countr=0;
      }
      break;
  }
  return state4;
}
  enum SM5_States {sm5_init, off5, on5};
  int SM5_Tick(int state5){
  //int count = 0;
  float t = dht.readTemperature();
  int f = dht.readTemperature(true);
  
  switch (state5){ //state transitions
    case sm5_init:
      state5 = off5;
      break;
    case off5:
      if(setTemp < f && systemPower == 1){
        state5 = on5;
      }
      if (setTemp == f){
        state5 = off5;
      }
      break;
    case on5:
      if(setTemp > f){
        state5 = off5;
      }
      if(setTemp == f){
        state5 = off5;
      }
      if(systemPower == 0){
        state5 = off5;
      }
      break;
  }

  switch (state5){ //state actions
    case sm5_init:
      digitalWrite(white,LOW);
      break;
    case off5:
      digitalWrite(white,LOW);
      break;
    case on5:
      if (countw == 0 || countw == 1 || countw == 2){
        digitalWrite(white,HIGH);
      }
      else{
        digitalWrite(white,LOW);
      }
      countw++;
      if(countw==4){
        countw=0;
      }
      break;
  }
  return state5;
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  
  //lcd.print("hello world");

  pinMode(blue, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(white, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(JS_BTN, INPUT_PULLUP); //value of 0 means its being pushed
  
  
  unsigned char i = 0;
  tasks[i].state = tempOff;
  tasks[i].period = 250;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;
  i++;
  tasks[i].state = sm_init;
  tasks[i].period = 40;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM2_Tick;
  i++;
  tasks[i].state = sm3_init;
  tasks[i].period = 100;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM3_Tick;
  i++;
  tasks[i].state = sm4_init;
  tasks[i].period = 5;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM4_Tick;
  i++;
  tasks[i].state = sm5_init;
  tasks[i].period = 60;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM5_Tick;
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i){
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis();
    }
  }
  
}