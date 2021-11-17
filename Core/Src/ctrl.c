#include "ctrl.h"
#include "pwm.h"
#include "gyro.h"
#include "main.h"

volatile float speedStraight = 0.0;
volatile bool anglePrepared = false;

volatile bool callbackflag = true;

/**
 * @brief 设置直行速度
 * @param s 直行速度
 */
void setSpeedStraight(float s) {
    speedStraight = s;
    pidLB.errint = 0;
    pidLF.errint = 0;
    pidRF.errint = 0;
    pidRB.errint = 0;
}

void setAngle(float ag) {
    pidAngle.goalstate = ag;
    pidAngle.errint = 0;
    anglePrepared = false;
}

/**
 * @brief 各轮速度获取回调函数
 */
void CTRL_Callback(void) {
    if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
        setSpeedStraight(0);
        setAngle(pidAngle.realstate);
        callbackflag = true;
    } else if (callbackflag) {
        setSpeedStraight(0);
        setAngle(pidAngle.realstate - 90);
        callbackflag = false;
    } else if (anglePrepared) {
        setSpeedStraight(40);
    }

    {
        float newstate = gyroAngle.z;
        float err = pidAngle.goalstate - newstate;
        err = err - round(err / 360) * 360;// 换算到[-180,180]范围
        if (err < ANGLE_PRE_DELTA && err > -ANGLE_PRE_DELTA) {
            anglePrepared = true;
        }

        pidAngle.pwm = calcAnglePWM(err, &pidAngle);
        if (err > ANGLE_QUICK_DELTA && err < -ANGLE_QUICK_DELTA) {
            pidAngle.pwm = err > 0 ? 1 : -1;
        }

        pidAngle.realstate = newstate;
    }

    pidLB.goalstate = speedStraight - pidAngle.pwm * MAX(speedStraight, MIN_ROT_DELTA_SPEED);
    pidLF.goalstate = speedStraight - pidAngle.pwm * MAX(speedStraight, MIN_ROT_DELTA_SPEED);
    pidRF.goalstate = speedStraight + pidAngle.pwm * MAX(speedStraight, MIN_ROT_DELTA_SPEED);
    pidRB.goalstate = speedStraight + pidAngle.pwm * MAX(speedStraight, MIN_ROT_DELTA_SPEED);
}
