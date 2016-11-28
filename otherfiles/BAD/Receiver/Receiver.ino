// Run ONLY ON MEGA
#include <Enotik.h>
#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield mot;
EnotikMaster e;

void setup() {
  pinMode(13, OUTPUT);
  mot.init();

  Serial.begin(9600);

  e.init_slave(0x55);
  //Enotik1.init_master(0x55);

  e.bind(1, Drive);
  e.bind(2, RunM);
  e.bind(3, Manipulator);
}

void loop() {
  digitalWrite(13, e.online);
  
  //Enotik.send_message(1, 1, 0);
  e.work();
}

void Drive(byte speed1, byte speed2)
{
  speed1 = (int)((speed1 - 127) * 3.15);
  speed2 = (int)((speed2 - 127) * 3.15);

  if (speed1 > 400)speed1 = 400;
  if (speed2 > 400)speed2 = 400;
  if (speed1 < -400)speed1 = -400;
  if (speed2 < -400)speed2 = -400;

  mot.setM1Speed(speed1);
  mot.setM2Speed(speed2);
}

//Start another activity; keys: 1, 2, 3, 4, 5
void RunM(byte i1, byte i2) {

}

//Manupulator action; keys: 6:OPEN, 7:CLOSE, 8:UP, 9:DOWN
void Manipulator(byte i1, byte i2) {

}

