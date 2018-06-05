#include <ArduinoJson.h>
#include <Servo.h>
#include <Scheduler.h>
#include "Omega.h"
#include "JointServo.h"
#include "OperateTelegram.h"

const int SERVO_CNT = 19;
const int MAINTE_PIN = 12;
const int S_CONF_IS_ARDUINO = 0;
const int S_CONF_PIN = 1;
const int S_CONF_ADJ_ANGLE = 2;
const int S_CONF_DEF_ANGLE = 3;

const int BUFFER_SIZE = 64;

int SERVO_CONF[SERVO_CNT][4];
int SERVO_AUTO_FB_ANGLES[SERVO_CNT][16];
int SERVO_AUTO_LR_ANGLES[SERVO_CNT][16];

OperateTelegram opeTele;
JointServo jServos[SERVO_CNT];
AutoModeStatus autoStatus;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("setup start"));

  Serial1.begin(115200);
  Serial.println(F("serial begin"));

  setupServoConfig();

  for (int id = 0; id < SERVO_CNT; id++)
  {
    jServos[id].isMyServo = SERVO_CONF[id][S_CONF_IS_ARDUINO];
    jServos[id].attach(SERVO_CONF[id][S_CONF_PIN]);
    jServos[id].moveNow(SERVO_CONF[id][S_CONF_DEF_ANGLE]);
    char buf[BUFFER_SIZE];
    sprintf(buf, "S%d is initialize", SERVO_CONF[id][S_CONF_PIN]);
    Serial.println(buf);
  }

  Scheduler.startLoop(loopSerialProc);
  Serial.println(F("start loop serial"));

  Scheduler.startLoop(loopServoControl);
  Serial.println(F("start loop control"));

  Scheduler.startLoop(loopServoControlAuto);
  Serial.println(F("start loop auto control"));

  Serial.println(F("ready"));
}

void loop()
{
  if (opeTele.status != TELE_STS_COMPLETED)
  {
    if (Serial1.available())
    {
      int inData = Serial1.read();
      Serial.print(inData);
      Serial.print(",");
      opeTele.add(inData);
    }
    else
      delay(100);
  }
  yield();
}

void loopSerialProc()
{
  if (opeTele.status == TELE_STS_COMPLETED)
  {
    autoStatus.isAutoMode = false;
    for (int i = 0; i < opeTele.servoList.size(); i++)
    {
      int speed = constrain(opeTele.servoList.at(i).speed, SERVO_CTRL_SPEED_MIN, SERVO_CTRL_SPEED_MAX);
      int servoID = constrain(opeTele.servoList.at(i).servoId, 0, SERVO_CNT - 1);
      int angle = constrain(opeTele.servoList.at(i).angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
      reserveServoControl(servoID, angle, speed);
    }
    opeTele.clear();
  }
  yield();
}

int reserveServoControl(int id, int angle, int speed)
{
  int cnt = jServos[id].move(angle, speed);
  return cnt;
}

void loopServoControl()
{
  for (int id = 0; id < SERVO_CNT; id++)
  {
    if (jServos[id].isMovement)
    {
      int angle = jServos[id].moveNext();

      if (!SERVO_CONF[id][S_CONF_IS_ARDUINO])
      {
        //request ESP-32
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.createObject();
        json["pin"] = jServos[id].pin;
        json["angle"] = angle;
        String tele;
        json.printTo(tele);
        Serial1.write(TELE_STX);
        Serial1.print(tele);
        Serial1.write(TELE_ETX);

        Serial.println("Request esp32");
      }
      char buf[BUFFER_SIZE];
      sprintf(buf, "now > S%d : %d", id, jServos[id].angles.nowPulse);
      Serial.println(buf);
    }
  }
  delay(SERVO_CTRL_INTERVAL);
}

void loopServoControlAuto()
{
  if (autoStatus.isAutoMode)
  {
    int cnt = 0;
    for (int id = 0; id < SERVO_CNT; id++)
    {
      int angle = SERVO_AUTO_FB_ANGLES[id][autoStatus.index];
      int _cnt = reserveServoControl(id, angle, autoStatus.speed);
      if (cnt < _cnt)
        cnt = _cnt;
    }

    if (autoStatus.isForward)
    {
      autoStatus.index++;
      if (autoStatus.index > 16)
        autoStatus.index = 0;
    }
    else if (autoStatus.isBack)
    {
      autoStatus.index--;
      if (autoStatus.index < 0)
        autoStatus.index = 16;
    }
    delay(cnt * SERVO_CTRL_INTERVAL);
  }
  else
    delay(100);
}

void setupServoConfig()
{
  String SD_DATA_SERVO_CONF[SERVO_CNT] = {
      "1,12,0,90",
      "1,2,0,90",
      "1,3,0,90",
      "1,4,0,90",
      "1,5,0,90",
      "1,6,0,90",
      "1,7,0,90",
      "0,16,0,160",
      "0,17,0,160",
      "0,5,0,160",
      "0,18,0,160",
      "0,19,0,160",
      "0,21,0,160",
      "0,12,0,160",
      "0,14,0,160",
      "0,27,0,160",
      "0,26,0,160",
      "0,25,0,160",
      "0,33,0,160"};

  String SD_DATA_AUTO_FB_ANGLES[SERVO_CNT] = {
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,100,104,107,110,100,96,93,90,78,72,68,65,78,83,87",
      "90,80,76,73,70,80,84,87,90,100,104,107,110,100,96,93",
      "90,103,108,112,115,103,97,93,90,80,76,73,70,80,84,87",
      "90,103,108,112,115,103,97,93,90,80,76,73,70,80,84,87",
      "90,80,76,73,70,80,84,87,90,100,104,107,110,100,96,93",
      "90,100,104,107,110,100,96,93,90,78,72,68,65,78,83,87",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160"};

  String SD_DATA_AUTO_LR_ANGLES[SERVO_CNT] = {
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160",
      "160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160"};

  for (int id = 0; id < SERVO_CNT; id++)
  {
    split(SD_DATA_SERVO_CONF[id], SERVO_CONF[id], ',');
    split(SD_DATA_AUTO_FB_ANGLES[id], SERVO_AUTO_FB_ANGLES[id], ',');
    split(SD_DATA_AUTO_LR_ANGLES[id], SERVO_AUTO_LR_ANGLES[id], ',');
  }
}

int split(String dist, int *dest, char delim)
{
  int cnt = 0;
  int idx = 0;
  for (cnt = 0; cnt < 255; cnt++)
  {
    int delimIdx = dist.indexOf(delim, idx);
    String str;
    if (delimIdx < 0)
      str = dist.substring(idx);
    else
      str = dist.substring(idx, delimIdx);
    dest[cnt] = str.toInt();

    if (delimIdx < 0)
      break;

    idx = delimIdx + 1;
  }

  return cnt + 1;
}
