//PINs
#define SERVOPINC  3
#define SERVOPINU  33

#define LLPIN      31
#define LaserPIN   32
#define B          37

#define US0_TP     23
#define US0_EP     22
#define US1_TP     25
#define US1_EP     24
#define US2_TP     27
#define US2_EP     26

#define LR         A3
#define LL         A2
#define LC         A4

//RANGES FOR SERVOS
#define S_CATCH    151
#define S_RELEASE  15
#define S_UP       180
#define S_DOWN     5

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

Servo srvc;
Servo srvu;
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

int s1pos = S_RELEASE;
int s2pos = S_UP;

int sc1 = 5;
int sc2 = 2;

int sc1_l = 1;
int sc2_l = 5;
int sc1_h = 1;
int sc2_h = 5;

unsigned long ttt = 0;

void setup() {
  //Serial.begin(9600);

  pinMode(13, OUTPUT);
  pinMode(LLPIN, OUTPUT);
  pinMode(LaserPIN, OUTPUT);
  pinMode(B, OUTPUT);

  mot.init();

  srvc.attach(SERVOPINC);
  srvc.write(S_RELEASE);
  srvu.attach(SERVOPINU);
  srvu.write(S_UP);

  link.bind(1, Drive);
  link.bind(2, RunM);
  link.bind(3, Manipulator);

  link.setOnDisconnect(Disconnect);
}

void loop() {
  digitalWrite(13, link.online);
  digitalWrite(LaserPIN, run4);

  Activity();
  updateServo();

  link.Run();

  if(!link.online && link.st0(1000)){
    digitalWrite(B, 1);
    delay(500);    
    digitalWrite(B, 0);
  }
}

void updateServo() {
  srvc.write(s1pos);
  srvu.write(s2pos);
}

void serialEvent1() {
  link.parseinput();
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
      Catch_c();
      updateServo();
      delay(300);
      Up_c();
      updateServo();
      run1 = 0;
      actID = 0;
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
    }
  }
}

//Start another activity; keys: 1, 2, 3, 4, 5
void RunM(byte i1, byte i2) {
  actID = int(i1);
  if (actID == 5) {
    speedmode = (speedmode == 0 ? 1 : (speedmode == 1 ? 2 : (speedmode == 2 ? 0 : 0)));
    sc1 = ((speedmode == 0 || speedmode == 1) ? sc1_l : sc1_h);
    sc2 = ((speedmode == 0 || speedmode == 1) ? sc2_l : sc2_h);
  }
  switch (i1) {
    case 1:
      run1 = !run1;
      if (run1) {
        Release();
        updateServo();
        Down();
        updateServo();
      }
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
      Release_u();
      break;
    case 7:
      Catch_u();
      break;
    case 8:
      Up_u();
      break;
    case 9:
      Down_u();
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
  s1pos = S_CATCH;
}
void Catch_c() {
  s1pos = ((S_CATCH - S_RELEASE) / 2);
}
void Release() {
  s1pos = S_RELEASE;
}
void Up() {
  s2pos = S_UP;
}
void Up_c() {
  s2pos = ((S_UP - S_DOWN) / 2);
}
void Down() {
  s2pos = S_DOWN;
}

//user servo cmd
void Catch_u() {
  s1pos += sc1;
  if (s1pos > S_CATCH)
    s1pos = S_CATCH;
}
void Release_u() {
  s1pos -= sc1;
  if (s1pos < S_RELEASE)
    s1pos = S_RELEASE;
}
void Up_u() {
  s2pos += sc2;
  if (s2pos > S_UP)
    s2pos = S_UP;
}
void Down_u() {
  s2pos -= sc2;
  if (s2pos < S_DOWN)
    s2pos = S_DOWN;
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
