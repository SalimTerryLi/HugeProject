#include "GY31.h"
#include <Arduino.h>

//private implement

enum LightFilter{
  Red,
  Green,
  Blue,
  AllPass
};

int Pin_S0,Pin_S1,Pin_S2,Pin_S3,Pin_OUT;
LightFilter currFilter;
unsigned long RedVal,GreenVal,BlueVal,LightVal;

unsigned long __pulseCount=0;
void __trigger_func(){
  //cli();
  ++__pulseCount;
  //sei();
}

void __nextFilter(){
  switch (currFilter){
  case Red:
    RedVal=__pulseCount;
    currFilter=Green;
    digitalWrite(Pin_S2,HIGH);
    digitalWrite(Pin_S3,HIGH);
    __pulseCount=0;
    break;
  case Green:
    GreenVal=__pulseCount;
    currFilter=Blue;
    digitalWrite(Pin_S2,LOW);
    digitalWrite(Pin_S3,HIGH);
    __pulseCount=0;
    break;
  case Blue:
    BlueVal=__pulseCount;
    currFilter=AllPass;
    digitalWrite(Pin_S2,HIGH);
    digitalWrite(Pin_S3,LOW);
    __pulseCount=0;
    break;
  case AllPass:
    LightVal=__pulseCount;
    currFilter=Red;
    digitalWrite(Pin_S2,LOW);
    digitalWrite(Pin_S3,LOW);
    __pulseCount=0;
    break;
  }
}

void __setupTimer(){
  cli();
  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 255;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  sei();
}

ISR(TIMER0_COMPA_vect){//timer0 interrupt 2kHz
  __nextFilter();
}

// public interface

/* Pin OUT must support interrupt */
void configureGY31(int S0,int S1,int S2,int S3,int OUT){
  Pin_S0=S0;
  Pin_S1=S1;
  Pin_S2=S2;
  Pin_S3=S3;
  Pin_OUT=OUT;
  
  pinMode(Pin_S0,OUTPUT);
  pinMode(Pin_S1,OUTPUT);
  pinMode(Pin_S2,OUTPUT);
  pinMode(Pin_S3,OUTPUT);
  pinMode(Pin_OUT,INPUT);
  attachInterrupt(digitalPinToInterrupt(Pin_OUT), __trigger_func, RISING);

  currFilter=AllPass;
  digitalWrite(Pin_S2,HIGH);
  digitalWrite(Pin_S3,LOW);

  __setupTimer();
}
unsigned long getRed(){return RedVal;}
unsigned long getGreen(){return GreenVal;}
unsigned long getBlue(){return BlueVal;}
unsigned long getLight(){return LightVal;}
void setOutputFreq(OutputFreq freq){
  switch (freq){
  case PowerDown:
    digitalWrite(Pin_S0,LOW);
    digitalWrite(Pin_S1,LOW);
    break;
  case Percent2:
    digitalWrite(Pin_S0,LOW);
    digitalWrite(Pin_S1,HIGH);
    break;
  case Percent20:
    digitalWrite(Pin_S0,HIGH);
    digitalWrite(Pin_S1,LOW);
    break;
  case Full:
    digitalWrite(Pin_S0,HIGH);
    digitalWrite(Pin_S1,HIGH);
    break;
  }
}
