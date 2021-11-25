#ifndef _CTRL_H_
#define _CTRL_H_

#include "pwm.h"
#include <stdbool.h>
#include <math.h>

#define MAX_DISTANCE 600
#define MIN_ROT_DELTA_SPEED 30
#define ANGLE_COMPLETE_DELTA 2.5
#define ANGLE_QUICK_DELTA 10

extern volatile float speedStraight;
extern volatile bool angleCompleted;

float getSpeedStraight();
float getRealSpeedStraight();
void setSpeedStraight(float s, float dis);
void setSpeedStraight0();
bool isDisCompleted();
float getAngle();
float getRealAngle();
void setAngle(float ag);
bool isAngleCompleted();

void CTRL_Callback(void);
void CTRL_After_Callback(void);

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))
// 换算到[-180,180]范围
inline float ANGLE_NORM(float x) {
    return x - round(x / 360) * 360;
}

#endif // !_CTRL_H_
