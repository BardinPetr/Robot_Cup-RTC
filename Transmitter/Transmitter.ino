#include <catlink.h>

#define BTN1            2
#define BTN2            3
#define BTN3            4
#define BTN4            5
#define BTNST           6

#define STICK_VERT      A2
#define STICK_GOR       A3

#define MSTICK_VERT      A2
#define MSTICK_GOR       A3

bool sflag = false;

CatLink link(0x22);

int BTNID() {
  if (digitalRead(BTN1)) return 1;
  else if (digitalRead(BTN2)) return 2;
  else if (digitalRead(BTN3)) return 3;
  else if (digitalRead(BTN4)) return 4;
  else if (digitalRead(BTNST)) return 5;
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

  Serial.begin(9600);

  link.bind(1, RecData);
}

void loop()
{
  bool state = BTNID();

  if (link.st0(200)){
    link.Read();
  }
  
  if (link.st1(10)) {
    if (state > 0 && state < 6) {
      link.Send(3, state, 0);
    }
    else if (state < 10) {
      link.Send(2, state, 0);
    }
    else {
      DriveSend();
    }
  }
  
  digitalWrite(13, link.online);
}

void RecData(byte i1, byte i2) {

}

void DriveSend()
{
  int vert_stick = analogRead(STICK_VERT);
  int gor_stick = analogRead(STICK_GOR);
  int motor1 = 0;
  int motor2 = 0;

  vert_stick = map(vert_stick, 1023, 0, -255, 255);
  gor_stick = map(gor_stick, 0, 1023, -255, 255);

  if (vert_stick > -5 && vert_stick < 5)vert_stick = 0;
  if (gor_stick > -5 && gor_stick < 5)gor_stick = 0;

  motor1 = vert_stick - gor_stick;
  motor2 = vert_stick + gor_stick;

  if (motor1 > 255)   motor1 = 255;
  if (motor2 > 255)   motor2 = 255;
  if (motor1 < -255)  motor1 = -255;
  if (motor2 < -255)  motor2 = -255;

  motor1 = map(motor1, -255, 255, 0, 255);
  motor2 = map(motor2, -255, 255, 0, 255);

  link.Send(1, motor1, motor2);
}
