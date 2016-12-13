#include <Wire.h>
#include <HMC58X3.h>
#include "DualVNH5019MotorShieldMega.h"
#include <Servo.h>
#include <NewPing.h>
#define US0_TP     23
#define US0_EP     22
#define US1_TP     25
#define US1_EP     24
#define US2_TP     27
#define US2_EP     26  
NewPing uS0(US0_TP, US0_EP, 200);
DualVNH5019MotorShield mot;
Servo srv;
HMC58X3 magn;
int fix = 0;

void setup(void) {
  Serial.begin(9600);
  Wire.begin();

  magn.init(false);
  magn.calibrate(1, 32);
  magn.setMode(0);
  setangle(90);
}
void loop() {
  int H = heading();
  if (H > (fix - 1) && H < (fix + 1)) {
    Serial.print("Finish");
    delay(500);
    setangle(90); 
  }
  Serial.println(H);
  delay(10);
}

void setangle(int angle) {
  fix = heading() + angle;
  if (fix > 360) fix -= 360;
}

int heading() {
  float fx, fy, fz;
  magn.getValues(&fx, &fy, &fz);

  float H = atan2(fy, fx);
  if (H < 0) {
    H += 2 * M_PI;
  }
  return (int)(H * 180 / M_PI);
}

