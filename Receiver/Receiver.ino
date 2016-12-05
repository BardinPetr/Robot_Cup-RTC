//PINs
#define SERVOPIN 3
#define LIGHTPIN 22
//RANGES FOR SERVOS
#define cMIN 0
#define cMAX 90
#define uNIN 90
#define uMAX 180

//INCLUDE
#include <catlink.h>
#include <Servo.h>
#include "DualVNH5019MotorShield.h"

DualVNH5019MotorShield mot;
CatLink link(0x22);
Servo man;

//SERVO VARs
int cservo = 0;
int uservo = 0;

int actID = 0;

//FLAGs
bool lightflag = 0;

void setup() {
  Serial.begin(9600);

  pinMode(13, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);

  mot.init();
  man.attach(SERVOPIN);

  link.bind(1, Drive);
  link.bind(2, RunM);
  link.bind(3, Manipulator);
}

void loop() {
  digitalWrite(13, link.online);
  if (link.st0(10)) {
    link.Read();
  }

  Activity();
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

void Activity() {
  switch (actID) {
    case 1:

      break;
    case 2:

      break;
    case 3:

      break;
    case 4:

      break;
    case 5:

      break;
    default:
      break;
  }
}

//Start another activity; keys: 1, 2, 3, 4, 5
void RunM(byte i1, byte i2) {
  if (i1 = actID)
    actID = 0;
  else
    actID = int(i1);
}

//Manupulator action; keys: 6:OPEN, 7:CLOSE, 8:UP, 9:DOWN
void Manipulator(byte i1, byte i2) {
  switch (i1) {
    case 6:
      Release();
      break;
    case 7:
      Catch();
      break;
    case 8:
      Up();
      break;
    case 9:
      Down();
      break;
  }
}

void Catch() {
  cservo += 10;
  if (cservo > cMAX) cservo = cMAX;

  man.write(cservo);
}

void Release() {
  cservo -= 10;
  if (cservo < cMIN) cservo = cMIN;

  man.write(cservo);
}

void Up() {
  uservo += 10;
  if (uservo > uMAX) uservo = uMAX;

  man.write(uservo);
}

void Down() {
  uservo -= 10;
  if (uservo < uMIN) uservo = uMIN;

  man.write(uservo);
}

void light_on() {digitalWrite(LIGHTPIN, 1);}
void light_off(){digitalWrite(LIGHTPIN, 0);}
