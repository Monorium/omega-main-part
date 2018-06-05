#ifndef OMEGA_H_INCLUDE
#define OMEGA_H_INCLUDE

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
