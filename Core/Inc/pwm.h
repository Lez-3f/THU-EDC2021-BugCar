#ifndef _PWM_H_
#define _PWM_H_
#include "tim.h"

#define PI 3.14159265f
// 四倍频
#define CNT2SP_BASE (UnitFreq / PIDPeriod * PI * 6.7)
#define CNT2SP_AVE (CNT2SP_BASE / 1062)

#define CNT2SP_LB (CNT2SP_BASE / 1062)
#define CNT2SP_LF (CNT2SP_BASE / 1062)
#define CNT2SP_RF (CNT2SP_BASE / 1060)
#define CNT2SP_RB (CNT2SP_BASE / 1061)


// 最大输出PWM占空比
#define MAXPWM 1.0
// 最小输出PWM占空比
#define MINPWM -1.0
// 最大车速
#define MAXSPEED 50.0
// 最小车速
#define MINSPEED -50.0

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float goalstate;
    float realstate;
    float pwm;
    float errint;
} PIDTypeDef;

extern PIDTypeDef pidLB;
extern PIDTypeDef pidLF;
extern PIDTypeDef pidRF;
extern PIDTypeDef pidRB;

void TIM_PID_Callback();
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt))) 

#endif // !_PWM_H_