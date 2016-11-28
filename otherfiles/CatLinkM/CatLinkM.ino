#include "catlink.h"
CatLink cl(0x22);

long int Clocks = 0, prevClocks = 0, Clocks2 = 0, prevClocks2 = 0;
bool soft_timer1(int);
bool soft_timer2(int);

bool st1(int stime)
{
  bool result;
  Clocks = millis();
  if (Clocks - prevClocks >= stime )
  {
    result = 1;
    prevClocks = Clocks;
  }
  else
    result = 0;

  return result;
}

bool st2(int stime)
{
  bool result;
  Clocks2 = millis();
  if (Clocks2 - prevClocks2 >= stime )
  {
    result = 1;
    prevClocks2 = Clocks2;
  }
  else
    result = 0;

  return result;
}

void fl(byte a, byte b){
  digitalWrite(13, 1);
  delay(1000);
  digitalWrite(13, 0);
  delay(400);
}

void fl1(byte a, byte b){
  digitalWrite(13, 1);
  delay(300);
  digitalWrite(13, 0);
  delay(300);
}

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  cl.bind(0, fl);
  cl.bind(1, fl1);
}

void loop() {
  while(!st1(2000));
  cl.Send(0, 10, 10);
  while(!st1(2000));
  cl.Send(1, 10, 10);

  //if(st2(100))
  //  cl.Read();

  //digitalWrite(13, cl.online);
}


