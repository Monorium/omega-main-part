#ifndef JOINTSERVOMANAGER_H_INCLUDE
#define JOINTSERVOMANAGER_H_INCLUDE

#include <QList.h>
#include "JointServo.h"

const int CS_SDCARD = 52;
const int MAX_SERVO_CNT = 19;
const String SERVO_CONFIG_NAME = "servo.jsn";
const String CONFIG_SERVO_DEFINE = "servo.define";
const String CONFIG_SERVO_DEFINE_ID = "id";
const String CONFIG_SERVO_DEFINE_ISMYSERVO = "isMyServo";
const String CONFIG_SERVO_DEFINE_PIN = "pin";
const String CONFIG_SERVO_DEFINE_ADJUST = "adjust.angle";
const String CONFIG_SERVO_DEFINE_DEFAULT = "default.angle";

class JointServoManager {
public:
  JointServoManager();
  void loadConfig();

  boolean isLoaded = false;
  QList<JointServo> servoList;

private:
  void loadDefaultConfig();
};

#endif
