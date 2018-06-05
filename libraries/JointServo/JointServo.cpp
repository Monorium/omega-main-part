#include <Arduino.h>
#include <Servo.h>
#include "JointServo.h"

JointServo::JointServo()
{
}

void JointServo::attach(int pinNo)
{
  pin = pinNo;
  if (isMyServo)
    servo.attach(pin);
}

void JointServo::moveNow(int angle)
{
  isMovement = false;
  angles.nowPulse = angleToPulse(angle);
  if (isMyServo)
    servo.writeMicroseconds(angles.nowPulse);
}

// 何回動くと目標座標に到達予定か返す
int JointServo::move(int angle, int speed)
{
  int pulse = angleToPulse(angle);
  if (angles.nowPulse == pulse) {
    isMovement = false;
    return 0;
  }
  angles.targetPulse = pulse;
  angles.onePulse = constrain(speed, SERVO_CTRL_SPEED_MIN, SERVO_CTRL_SPEED_MAX);
  if (angles.targetPulse < angles.nowPulse) {
    angles.onePulse = angles.onePulse * -1;
  }
  isMovement = true;

  float safix = (float)(angles.targetPulse - angles.nowPulse);
  return (int)(abs(safix / angles.onePulse) + 0.5);
}

int JointServo::moveNext()
{
  if (isMovement) {
    angles.nowPulse += angles.onePulse;
    if ((angles.onePulse >= 0) && (angles.nowPulse >= angles.targetPulse))
    {
      angles.nowPulse = angles.targetPulse;
      isMovement = false;
    }
    else if ((angles.onePulse < 0) && (angles.nowPulse <= angles.targetPulse))
    {
      angles.nowPulse = angles.targetPulse;
      isMovement = false;
    }

    if (isMyServo)
      servo.write(angles.nowPulse);
  }
  return angles.nowPulse;
}

int JointServo::angleToPulse(int angle)
{
  int _angle = constrain(angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
  return (int)map(_angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX, SERVO_PULS_MIN, SERVO_PULS_MAX);
}

int JointServo::pulseToAngle(int pulse)
{
  int _pulse = constrain(pulse, SERVO_PULS_MIN, SERVO_PULS_MAX);
  return (int)map(_pulse, SERVO_PULS_MIN, SERVO_PULS_MAX, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
}
