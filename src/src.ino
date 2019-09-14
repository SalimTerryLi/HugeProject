#include "GY31.h"
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define OUT 2

#define LGRED 9
#define LGYELLOW 10
#define LGBLUE 11

#define SLIDEPWR 13

unsigned long RedRef = 30, GreenRef = 25, BlueRef = 30;
unsigned long RedStep = 40, GreenStep = 30, BlueStep = 32;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
unsigned long lastTickMS = 0;


void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  pulselength /= 4096;  // 12 bits of resolution
  pulse *= 1000000;  // convert to us
  pulse /= pulselength;
  pwm.setPWM(n, 0, pulse);
}

void setup() {
  Serial.begin(9600);
  pinMode(LGRED, INPUT);
  pinMode(LGYELLOW, INPUT);
  pinMode(LGBLUE, INPUT);
  pinMode(SLIDEPWR,OUTPUT);
  digitalWrite(SLIDEPWR,LOW);
  // put your setup code here, to run once:
  configureGY31(S0, S1, S2, S3, OUT);
  setOutputFreq(Percent20);
  pwm.begin();
  pwm.setPWMFreq(60);
  lastTickMS = millis();
}

int itemColor = -1;
bool detectItem() {
  bool haveRed = false, haveGreen = false, haveBlue = false;
  unsigned long tmp;
  tmp = getRed();
  if (tmp > RedRef + RedStep) {
    haveRed = true;
  }
  tmp = getGreen();
  if (tmp > GreenRef + GreenStep) {
    haveGreen = true;
  }
  tmp = getBlue();
  if (tmp > BlueRef + BlueStep) {
    haveBlue = true;
  }
  itemColor = 1 * haveRed + 2 * haveGreen + 4 * haveBlue;

  /*Serial.print(haveRed);
  Serial.print(" ");
  Serial.print(haveGreen);
  Serial.print(" ");
  Serial.print(haveBlue);
  Serial.println();*/
  return haveRed || haveGreen || haveBlue;
}

bool isItem = false;

bool isPassed = false;

void push(int index) {
  //setServoPulse(index,400);
  Serial.print(index);
  isPassed = true;
}
int samplecount = 0;
int redC = 0, greenC = 0, blueC = 0, yellowC = 0;
void loop() {

  if(Serial.read()=='!'){digitalWrite(SLIDEPWR,LOW);}

  if (millis() - lastTickMS >= 1) {setServoPulse(0,250);
  setServoPulse(1,390);
  setServoPulse(2,250);

    /*if ((isItem == false) && detectItem()) {
      isItem = true;
      Serial.println("New Item");
      }*/
    if (isItem == false) {
      if (detectItem()) {
        samplecount++;
        switch (itemColor) {
          case 1:
            redC++;
            break;
          case 2:
            greenC++;
            break;
          case 3:
            yellowC++;
            break;
          case 4:
            blueC++;
            break;
        }
      }
      if (samplecount >= 3) {
        isItem = true;
        digitalWrite(SLIDEPWR,HIGH);
        if (yellowC != 0) {
          itemColor = 3;
        }
        samplecount = 0;
        redC = 0;
        greenC = 0;
        blueC = 0;
        yellowC = 0;

        //Serial.println("New Item");
      }
    }
      /*Serial.print(getRed());
      Serial.print(" ");
      Serial.print(getGreen());
      Serial.print(" ");
      Serial.print(getBlue());
      Serial.print(" ");
      Serial.print(getLight());
      Serial.println();*/
    //setServoPulse(0,400);
    if (isItem == true) {
      if (digitalRead(LGRED) == HIGH) {
        if (itemColor == 1) {
          push(0);
        }
      }
      if (digitalRead(LGYELLOW) == HIGH) {
        if (itemColor == 3) {
          push(1);
        }
      }
      if (digitalRead(LGBLUE) == HIGH) {
        if (itemColor == 4) {
          push(2);
        }else if(itemColor == 2){
          push(3);
        }
        isPassed = true;
      }
    }

    if ((isItem == true) && (isPassed == true)) {
      //Serial.println("Passed.");
      isItem = false;
      isPassed = false;
    }

    // sys loop ctrl.
    lastTickMS = millis();
  }
}
