#include <Arduino.h>
#include <ArduinoJson.h>
#include "OperateTelegram.h"

OperateTelegram::OperateTelegram()
{
  clear();
}

void OperateTelegram::clear()
{
  status = TELE_STS_WAIT_START;
  data = "";
  servoList.clear();
}

void OperateTelegram::add(char teleData)
{
  switch (teleData)
  {
    case TELE_STX:
      clear();
      status = TELE_STS_RECVING;
      break;
    case TELE_ETX:
      if (analyzeData())
        status = TELE_STS_COMPLETED;
      else
        clear();
      break;
    default:
      if (status == TELE_STS_RECVING)
        data += String(teleData);
      break;
  }
}

boolean OperateTelegram::analyzeData()
{
  Serial.println(data);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &jsonData = jsonBuffer.parseObject(data);
  if (!jsonData.success())
  {
    Serial.println("parseObject() failed");
    return false;
  }

  int speed = jsonData[TELE_KEY_SPEED];
  JsonArray &servoes = jsonData.get<JsonArray>(TELE_KEY_SERVO);
  for (int i = 0; i < servoes.size(); i++)
  {
    ServoControlData servoData;
    servoData.speed = speed;
    servoData.servoId = servoes[i][TELE_KEY_ID];
    servoData.angle = servoes[i][TELE_KEY_ANGLE];
    servoList.push_back(servoData);
  }

  return true;
}
