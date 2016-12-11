//PINs
#define SERVOPIN   3
#define LIGHTPIN   31
#define US0_TP     23
#define US1_EP     22
//RANGES FOR SERVOS
#define DOWN       85
#define MID        35
#define UP         0

//INCLUDE
#include <catlink.h>
#include "DualVNH5019MotorShieldMega.h"
#include <Servo.h>
#include <NewPing.h>

NewPing uS0(US0_TP, US1_EP, 200);
DualVNH5019MotorShield mot;
Servo srv;
CatLink link(0x22, Serial1);

//SERVO VARs
int cservo = 0;
int uservo = 0;

int actID = 0;

//FLAGs
bool lightflag = 0;
int speedmode = 0;

bool run1 = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(13, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);

  mot.init();
  srv.attach(SERVOPIN);

  link.bind(1, Drive);
  link.bind(2, RunM);
  link.bind(3, Manipulator);
}

void loop() {
  digitalWrite(13, link.online);

  if (link.st0(100)) {
    link.Read();
  }

  Activity();
}

void Drive(byte sp1, byte sp2)
{
  digitalWrite(LIGHTPIN, 0);

  int speed1 = (int)(sp1);
  int speed2 = (int)(sp2);

  if (speedmode == 0) {
    speed1 = (int)((speed1 - 127) * 2);
    speed2 = (int)((speed2 - 127) * 2);

    if (speed1 > 100)   speed1 = 100;
    if (speed2 > 100)   speed2 = 100;
    if (speed1 < -100)  speed1 = -100;
    if (speed2 < -100)  speed2 = -100;
  }
  else if (speedmode == 1) {
    speed1 = (int)((speed1 - 127) * 2);
    speed2 = (int)((speed2 - 127) * 2);

    if (speed1 > 200)   speed1 = 200;
    if (speed2 > 200)   speed2 = 200;
    if (speed1 < -200)  speed1 = -200;
    if (speed2 < -200)  speed2 = -200;
  }
  else { //TURBO!!
    speed1 = (int)((speed1 - 127) * 3.15);
    speed2 = (int)((speed2 - 127) * 3.15);

    if (speed1 > 400)   speed1 = 400;
    if (speed2 > 400)   speed2 = 400;
    if (speed1 < -400)  speed1 = -400;
    if (speed2 < -400)  speed2 = -400;
  }

  if (speed1 < 10 && speed1 > -10) speed1 = 0;
  if (speed2 < 10 && speed2 > -10) speed2  = 0;

  mot.setM1Speed(-speed1);
  mot.setM2Speed(-speed2);
}

void motors(int i1, int i2) {
  mot.setM1Speed(-i1);
  mot.setM2Speed(-i2);
}

void Activity() {
  switch (actID) {
    case 1:
      tobanka();
      break;
    case 2:

      break;
    case 3:

      break;
    default:
      break;
  }
}

void tobanka() {
  if (run1) {
    motors(50, 50);
    if (ping0() < 4) {
      motors(0, 0);
      Catch();
      delay(100);
      Up();
      run1 = 0;
      actID = 0;
    }
  }
}

//Start another activity; keys: 1, 2, 3, 4, 5
void RunM(byte i1, byte i2) {
  digitalWrite(LIGHTPIN, 1);
  actID = int(i1);
  if (actID == 5) {
    speedmode = (speedmode == 1 ? 0:1);
  }
  if (actID == 4) {
    speedmode = (speedmode == 2 ? 1:2);
  }
  switch (i1) {
    case 1:
      run1 =1;
      break;
    case 2:

      break;
    case 3:

      break;
    default:
      break;
  }
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
  srv.write(MID);
}

void Release() {
  srv.write(DOWN);
}

void Up() {
  srv.write(UP);
}

void Down() {
  srv.write(MID);
}

int ping0() {
  return uS0.ping() / US_ROUNDTRIP_CM;
}
void light_off() {
  digitalWrite(LIGHTPIN, 0);
}

