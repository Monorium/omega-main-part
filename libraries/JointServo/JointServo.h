#ifndef JOINTSERVO_H_INCLUDE
#define JOINTSERVO_H_INCLUDE

#include <Servo.h>

const int SERVO_PULS_MIN = 540;
const int SERVO_PULS_MAX = 2400;
const int SERVO_ANGLE_MIN = 0;
const int SERVO_ANGLE_MAX = 180;
const int SERVO_CTRL_SPEED_MIN = 1;
const int SERVO_CTRL_SPEED_MAX = 254;
const int SERVO_CTRL_INTERVAL = 10;

struct Angles_t {
  int nowPulse = 90;
  int targetPulse = 90;
  int onePulse = 0;
};
typedef Angles_t Angles;

class JointServo {
public:
  JointServo();

  boolean isMyServo = true;
  boolean isMovement = false;
  Angles angles;
  int pin = 0;
  int adjAngle = 0;
  int defAngle = 0;

  void attach(int pinNo);
  void moveNow(int angle);
  int move(int angle, int speed);
  int moveNext();
  int angleToPulse(int andle);
  int pulseToAngle(int pulse);

private:
  Servo servo;
};

#endif
