//PINs
#define SERVOPINC  3
#define SERVOPINU  3 //TODO

#define LLPIN      31
#define LaserPIN   32

#define US0_TP     23
#define US0_EP     22
#define US1_TP     25
#define US1_EP     24
#define US2_TP     27
#define US2_EP     26

#define LR         A3
#define LL         A2
#define LC         A4

//RANGES FOR SERVOS //TODO
#define S_CATCH    180
#define S_RELEASE  0
#define S_UP       180
#define S_S_DOWN   0

//INCLUDE
#include <catlink.h>
#include "DualVNH5019MotorShieldMega.h"
#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <HMC58X3.h>

NewPing uS0(US0_TP, US0_EP, 200);
NewPing uS1(US1_TP, US1_EP, 200);
NewPing uS2(US2_TP, US2_EP, 200);

DualVNH5019MotorShield mot;

Servo srvC;
Servo srvU;
HMC58X3 mag;

CatLink link(0x22, Serial1);

int cservo = 0;
int uservo = 0;

int actID = 0;

bool lightflag = 0;
int speedmode = 1;

bool run1 = 0;
bool run2 = 0;
bool run3 = 0;
bool run4 = 0;

int fix = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(13, OUTPUT);
  pinMode(LLPIN, OUTPUT);
  pinMode(LaserPIN, OUTPUT);

  mot.init();

  srvc.attach(SERVOPINC);
  srvc.write(S_RELEASE);
  srvu.attach(SERVOPINU);
  srvu.write(S_DOWN);

  //Wire.begin();
  //mag.init(false);
  //mag.calibrate(1, 32);
  //mag.setMode(0);
  //setangle(90);


  link.bind(1, Drive);
  link.bind(2, RunM);
  link.bind(3, Manipulator);

  link.setOnDisconnect(Disconnect);
}

void loop() {
  digitalWrite(13, link.online);
  digitalWrite(LaserPIN, run4);

  if (link.st0(100)) {
    link.Read();
  }

  Activity();
}

void labirint() {
  if (run3) {
    if (ping1() > 10) {
      motors(50, -50);
      delay(1000);
      motors(0, 0);
    }
    else if (ping0() > 10) {
      motors(50, 50);
      delay(1000);
      motors(0, 0);
    }
    else {
      motors(50, -50);
      delay(1000);
      motors(0, 0);
    }
  }
}

void setangle(int angle) {
  fix = heading() + angle;
  if (fix > 360) fix -= 360;
}

int heading() {
  float fx, fy, fz;
  mag.getValues(&fx, &fy, &fz);

  float H = atan2(fy, fx);
  if (H < 0) {
    H += 2 * M_PI;
  }
  return (int)(H * 180 / M_PI);
}

void line_all() {
  int a = getlineC();
  if (a > 60)
    line_norm();
  else
    line_inv();
}


void line_inv() {
  if (run2) {
    digitalWrite(LLPIN, 1);
    int sensor1 = getlineL();
    int sensor2 = getlineR();

    if (sensor1 > 60) {
      if (sensor2 > 60) {
        motors(70, 70);
      }
      else {
        motors(-20, 150);
      }
    }
    else {
      if (sensor2 > 60) {
        motors(150, -20);
      }
      else {
        motors(70, 70);
      }
    }
  }
  else
    digitalWrite(LLPIN, 0);
}

void line_norm() {
  if (run2) {
    digitalWrite(LLPIN, 1);
    int sensor1 = getlineL();
    int sensor2 = getlineR();

    if (sensor1 > 60) {
      if (sensor2 > 60) {
        motors(70, 70);
      }
      else {
        motors(150, -20);
      }
    }
    else {
      if (sensor2 > 60) {
        motors(-20, 150);
      }
      else {
        motors(70, 70);
      }
    }
  }
  else
    digitalWrite(LLPIN, 0);
}

void tobanka() {
  if (run1) {
    motors(50, 50);
    if (ping0() < 4) {
      motors(0, 0);
      Catch();
      delay(500);
      Up();
      run1 = 0;
      actID = 0;
      link.clearBuffer();
    }
  }
}

void tostenka() {
  if (run3) {
    motors(150, 150);
    if (ping0() < 10) {
      motors(0, 0);
      run1 = 0;
      actID = 0;
      link.clearBuffer();
    }
  }
}

//Start another activity; keys: 1, 2, 3, 4, 5
void RunM(byte i1, byte i2) {
  actID = int(i1);
  if (actID == 5) {
    speedmode = (speedmode == 0 ? 1 : (speedmode == 1 ? 2 : (speedmode == 2 ? 0 : 0)));
  }
  switch (i1) {
    case 1:
      run1 = !run1;
      Release();
      break;
    case 2:
      run2 = !run2;
      break;
    case 3:
      run3 = !run3;
      break;
    case 4:
      run4 = !run4;
      break;
    default:
      break;
  }
}

void Activity() {
  switch (actID) {
    case 1:
      tobanka();
      break;
    case 2:
      line_all();
      break;
    case 3:
      tostenka();
      break;
    case 4:
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

void Drive(byte sp1, byte sp2)
{
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

void Disconnect() {
  motors(0, 0);
}

void Catch() {
  srvc.write(S_CATCH);
  delay(10);
}
void Release() {
  srvc.write(S_RELEASE);
  delay(10);
}
void Up() {
  srvu.write(S_UP);
  delay(10);
}
void Down() {
  srvu.write(S_DOWN);
  delay(10);
}

int ping0() {
  return uS0.ping() / US_ROUNDTRIP_CM;
}
int ping1() {
  return uS1.ping() / US_ROUNDTRIP_CM;
}
int ping2() {
  return uS2.ping() / US_ROUNDTRIP_CM;
}

int getlineR() {
  return map(analogRead(LR), 540, 650, 0, 100);
}
int getlineL() {
  return map(analogRead(LL), 610, 680, 0, 100);
}
int getlineC() {
  return map(analogRead(LC), 530, 580, 0, 100);
}
