#include <catlink.h>
#include <SoftwareSerial.h>

#define BTN1            4
#define BTN2            5
#define BTN3            6
#define BTN4            7
#define BTNST           18

#define LEDA            8
#define LEDB            9
#define LEDC            10
#define LEDD            11

#define STICK_VERT      A3
#define STICK_GOR       A2

#define MSTICK_VERT      A0
#define MSTICK_GOR       A1

bool sflag = false;
int speedmode = 0;

SoftwareSerial sr(3, 2);
CatLink link(0x22, sr);

int BTNID() {
  if (!digitalRead(BTN1)) return 1;
  else if (!digitalRead(BTN2)) return 2;
  else if (!digitalRead(BTN3)) return 3;
  else if (!digitalRead(BTN4)){speedmode = (speedmode == 2 ? 1:2); return 4;}
  else if (!digitalRead(BTNST)) {speedmode = (speedmode == 1 ? 0:1); return 5;}
  else if (analogRead(MSTICK_GOR) > 800) return 6;
  else if (analogRead(MSTICK_GOR) < 200) return 7;
  else if (analogRead(MSTICK_VERT) > 800) return 8;
  else if (analogRead(MSTICK_VERT) < 200) return 9;
  else return 0;
}

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(BTNST, INPUT_PULLUP);

  pinMode(LEDA, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDC, OUTPUT);
  pinMode(LEDD, OUTPUT);

  Serial.begin(9600);

  link.bind(1, RecData);
}

void loop() 
{
  int state = BTNID();
  
  if (state > 0 && state < 6) {
    link.Send(2, state, 0);
    delay(300);
  }
  else if (state > 5 && state < 10) {
    link.Send(3, state, 0);
  }
  
  else {
    if (link.st1(100)) {
      DriveSend();
    }
  }
  
  digitalWrite(LEDA, (speedmode == 2 ? 1:0));
  digitalWrite(LEDB, (speedmode == 1 ? 1:0));
}

void RecData(byte i1, byte i2) {
  //digitalWrite(LEDB, link.online);
}

void DriveSend()
{
  int vert_stick = analogRead(STICK_VERT);
  int gor_stick = analogRead(STICK_GOR);
  int motor1 = 0;
  int motor2 = 0;

  vert_stick = map(vert_stick, 0, 1023, -255, 255);
  gor_stick = map(gor_stick, 0, 1023, -255, 255);

  //if (vert_stick > -5 && vert_stick < 5)vert_stick = 0;
  //if (gor_stick > -5 && gor_stick < 5)gor_stick = 0;

  motor1 = vert_stick - gor_stick;
  motor2 = vert_stick + gor_stick;

  if (motor1 > 255)   motor1 = 255;
  if (motor2 > 255)   motor2 = 255;
  if (motor1 < -255)  motor1 = -255;
  if (motor2 < -255)  motor2 = -255;

  motor1 = map(motor1, -255, 255, 0, 255);
  motor2 = map(motor2, -255, 255, 0, 255);

  if (motor1 > 255)   motor1 = 255;
  if (motor2 > 255)   motor2 = 255;
  if (motor1 < 0)  motor1 = 0;
  if (motor2 < 0)  motor2 = 0;

  if (motor2 < (motor1 + 10) && motor2 > (motor1 - 10)) motor2 = motor1;

  //  Serial.print((int)((motor1 - 127) * 2));
  //  Serial.print(" ");
  //  Serial.print((int)((motor2 - 127) * 2));
  //  Serial.print("    ");
  //
  //  Serial.print(motor1);
  //  Serial.print(" ");
  //  Serial.println(motor2);

  link.Send(1, motor1, motor2);
}
