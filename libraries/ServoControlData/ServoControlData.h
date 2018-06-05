#ifndef SERVOCONTROLDATA_H_INCLUDE
#define SERVOCONTROLDATA_H_INCLUDE

class ServoControlData
{
  public:
    ServoControlData();

    int speed = 100;
    int servoId = 0;
    int angle = 90;
};

#endif
