#ifndef OMEGA_H_INCLUDE
#define OMEGA_H_INCLUDE

const int MAINTE_PIN = 12;
const int BUFFER_SIZE = 64;

struct AutoMode_t
{
    boolean isAutoMode = false;
    int index = 0;
    int speed = 0;
    boolean isForward = false;
    boolean isBack = false;
    boolean isLeft = false;
    boolean isRight = false;
};
typedef AutoMode_t AutoModeStatus;

#endif
