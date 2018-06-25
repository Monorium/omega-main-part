#include <ArduinoJson.h>
#include <Scheduler.h>
#include "JointServoManager.h"
#include "OperateTelegram.h"
#include "Omega.h"

OperateTelegram opeTele;
JointServoManager servoMan;

// 初期化処理
void setup()
{
  // コンソール通信初期化
  Serial.begin(115200);
  Serial.println(F("setup start"));

  // ESP32通信初期化
  Serial1.begin(115200);
  Serial.println(F("serial begin"));
  delay(3000);

  // サーボ管理クラス初期化
  servoMan.loadConfig();
  Serial.println(servoMan.servoList.size());
  for (int id = 0; id < servoMan.servoList.size(); id++)
  {
    // ESP33の場合は制御しておく
    if (!servoMan.servoList.at(id).isMyServo)
      controlEspServo(servoMan.servoList.at(id).pin, servoMan.servoList.at(id).defAngle);
  }

  // 各種タイマー起動
  // ESP32受信電文処理
  Scheduler.startLoop(loopSerialProc);
  Serial.println(F("start loop serial"));
  // サーボ制御処理（スピード制御のため）
  Scheduler.startLoop(loopServoControl);
  Serial.println(F("start loop control"));

  Serial.println(F("ready"));
}

// メインループ（ESP32通信処理）
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

// ESP32受信電文処理
void loopSerialProc()
{
  if (opeTele.status == TELE_STS_COMPLETED)
  {
    for (int i = 0; i < opeTele.servoList.size(); i++)
    {
      int speed = constrain(opeTele.servoList.at(i).speed, SERVO_CTRL_SPEED_MIN, SERVO_CTRL_SPEED_MAX);
      int servoID = constrain(opeTele.servoList.at(i).servoId, 0, servoMan.servoList.size() - 1);
      int angle = constrain(opeTele.servoList.at(i).angle, SERVO_ANGLE_MIN, SERVO_ANGLE_MAX);
      reserveServoControl(servoID, angle, speed);
    }
    opeTele.clear();
  }
  yield();
}

// サーボ制御処理への予約処理
int reserveServoControl(int id, int angle, int speed)
{
  return servoMan.servoList.at(id).move(angle, speed);
}

// サーボ制御処理（スピード制御のため）
void loopServoControl()
{
  for (int id = 0; id < servoMan.servoList.size(); id++)
  {
    if (servoMan.servoList.at(id).isMovement)
    {
      int pulse = servoMan.servoList.at(id).moveNext();
      if (!servoMan.servoList.at(id).isMyServo)
        controlEspServo(servoMan.servoList.at(id).pin, servoMan.servoList.at(id).pulseToAngle(pulse));
    }
  }
  delay(SERVO_CTRL_INTERVAL);
}

// ESP32へサーボ制御要求を送信
void controlEspServo(int pin, int angle)
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["pin"] = pin;
  json["angle"] = angle;
  String tele;
  json.printTo(tele);
  Serial1.write(TELE_STX);
  Serial1.print(tele);
  Serial1.write(TELE_ETX);

  Serial.print(F("Request esp32 -> "));
  Serial.println(tele);
}
