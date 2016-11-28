#include "link.h"

int pack[3];
int reads[7];
long int Clocks = 0, prevClocks = 0, Clocksm2 = 0, prevClocksm2 = 0;
bool soft_timer1(int);
bool soft_timer2(int);

void setup()
{
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  apc_SetUp();
}
void d(byte i, byte j) {};

void loop()
{
  bind(0, d);
  handlers[0].handler(0, 1);
  //if ( soft_timer1(100) == 1)
  //{
  //  apc_Send(1, 2, 3);
  //}
  //apc_Read();
  //digitalWrite(13, online());
}



bool soft_timer1(int stime)
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

bool soft_timer2(int stime)
{
  bool result;
  Clocksm2 = millis();
  if (Clocksm2 - prevClocksm2 >= stime )
  {
    result = 1;
    prevClocksm2 = Clocksm2;
  }
  else
    result = 0;

  return result;
}
