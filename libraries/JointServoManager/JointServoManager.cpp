#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include "JointServo.h"
#include "JointServoManager.h"

// サーボ管理クラス
JointServoManager::JointServoManager()
{
}

// SDカードからservo.jsonを読み込む
void JointServoManager::loadConfig()
{
  if (isLoaded)
    return;

  // サーボ情報の格納用配列を初期化
  for (int i = 0; i < MAX_SERVO_CNT; i++)
  {
    JointServo servo = JointServo();
    servoList.push_back(servo);
  }

  if (!SD.begin(CS_SDCARD))
  {
    Serial.println(F("SDカード接続エラー"));
    loadDefaultConfig();
    return;
  }
  Serial.println(F("SDカード接続"));

  // 設定ファイルアクセス
  if (!SD.exists(SERVO_CONFIG_NAME))
  {
    Serial.println(F("設定ファイルなし"));
    loadDefaultConfig();
    return;
  }
  File config = SD.open(SERVO_CONFIG_NAME, FILE_READ);
  if (!config)
  {
    Serial.println(F("設定ファイルオープンエラー"));
    config.close();
    loadDefaultConfig();
    return;
  }

  String conf = "";
  while (config.available())
  {
    conf += config.readStringUntil('\n');
  }
  Serial.println(conf);

  DynamicJsonBuffer jsonBuffer;
  JsonObject &jsonConf = jsonBuffer.parseObject(conf);
  if (!jsonConf.success())
  {
    Serial.println(F("設定ファイル解析エラー"));
    config.close();
    loadDefaultConfig();
    return;
  }

  // 設定からJointServoオブジェクトを更新
  JsonArray &servoes = jsonConf.get<JsonArray>(CONFIG_SERVO_DEFINE);
  for (int i = 0; i < servoes.size(); i++)
  {
    JsonObject &obj = servoes.get<JsonObject>(i);
    int id = obj.get<unsigned int>(CONFIG_SERVO_DEFINE_ID);
    if (id >= MAX_SERVO_CNT)
      continue;

    servoList.at(id).isMyServo = obj.get<bool>(CONFIG_SERVO_DEFINE_ISMYSERVO);
    servoList.at(id).adjAngle = obj.get<signed int>(CONFIG_SERVO_DEFINE_ADJUST);
    servoList.at(id).defAngle = obj.get<signed int>(CONFIG_SERVO_DEFINE_DEFAULT);
    servoList.at(id).attach(obj.get<unsigned int>(CONFIG_SERVO_DEFINE_PIN));
    servoList.at(id).moveNow(servoList.at(id).defAngle);
  }

  config.close();
  isLoaded = true;
}

// 設定ファイルエラーのときの初期化処理
void JointServoManager::loadDefaultConfig()
{
  Serial.println(F("初期値で設定"));
  isLoaded = true;
}
