// Run ONLY ON MEGA
#include <catlink.h>
#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield mot;
CatLink link(0x22);

void setup() {
  pinMode(13, OUTPUT);
  mot.init();

  Serial.begin(9600);

  link.bind(1, Drive);
  link.bind(2, RunM);
  link.bind(3, Manipulator);
}

void loop() {
  digitalWrite(13, link.online);
  if(link.st0(10))
    link.Read();
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

