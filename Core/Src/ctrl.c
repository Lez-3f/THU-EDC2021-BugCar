#include "ctrl.h"
#include "pwm.h"
#include "gyro.h"
#include "main.h"
#include <stdbool.h>

volatile float speedStraight = 0.0;

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
}

/**
 * @brief 各轮速度获取回调函数
 */
void CTRL_Callback(void) {
    static bool flag = true;
    if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
        setSpeedStraight(0);
        setAngle(pidAngle.realstate);
        flag = true;
    } else if (flag) {
        setSpeedStraight(20);
        // setAngle(pidAngle.realstate + 90);
        flag = false;
    }

    {
        float newstate = gyroAngle.z;
        pidAngle.pwm = calcAnglePWM(newstate, &pidAngle);
        pidAngle.realstate = newstate;
    }

    pidLB.goalstate = speedStraight - pidAngle.pwm * MAX_ROT_DELTA_SPEED;
    pidLF.goalstate = speedStraight - pidAngle.pwm * MAX_ROT_DELTA_SPEED;
    pidRF.goalstate = speedStraight + pidAngle.pwm * MAX_ROT_DELTA_SPEED;
    pidRB.goalstate = speedStraight + pidAngle.pwm * MAX_ROT_DELTA_SPEED;
}
