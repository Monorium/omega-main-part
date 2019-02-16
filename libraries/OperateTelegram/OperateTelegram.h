#ifndef OPERATETELEGRAM_H_INCLUDE
#define OPERATETELEGRAM_H_INCLUDE

#include <QList.h>
#include "ServoControlData.h"

const int TELE_STX = 0x2;
const int TELE_ETX = 0x3;

const int TELE_STS_WAIT_START = 0;
const int TELE_STS_RECVING = 1;
const int TELE_STS_COMPLETED = 99;

const int TELE_COMMAND_AUTO = 1;
const int TELE_COMMAND_MANUAL = 2;

const int TELE_ACTION_FOWARD_BIT = 1;
const int TELE_ACTION_BACK_BIT = 2;
const int TELE_ACTION_LEFT_BIT = 4;
const int TELE_ACTION_RIGHT_BIT = 8;

const String TELE_KEY_SPEED = "speed";
const String TELE_KEY_SERVO = "servo";
const String TELE_KEY_ID = "id";
const String TELE_KEY_ANGLE = "angle";

class OperateTelegram
{
public:
  OperateTelegram();

  int status = TELE_STS_WAIT_START;
  QList<ServoControlData> servoList;

  void clear();
  void add(char teleData);

private:
  String data = "";

  boolean analyzeData();
};

#endif
