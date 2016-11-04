#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();;
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);;
void setup()
{
  AFMS.begin();
}

void loop()
{
  myMotor->setSpeed(10);
}
