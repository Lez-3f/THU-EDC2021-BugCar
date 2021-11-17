#ifndef _CTRL_H_
#define _CTRL_H_

#include <stdbool.h>

#define MIN_ROT_DELTA_SPEED 20
#define ANGLE_PRE_DELTA 3
#define ANGLE_QUICK_DELTA 10

extern volatile float speedStraight;
extern volatile bool anglePrepared;

void setSpeedStraight(float s);

void CTRL_Callback(void);// 轮速获取

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#endif // !_CTRL_H_
