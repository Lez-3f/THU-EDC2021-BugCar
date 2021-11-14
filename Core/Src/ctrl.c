#include "ctrl.h"
#include "pwm.h"
#include "main.h"
#include <stdbool.h>

volatile float speedStraight = 0.0;
volatile float goalAngle = 0.0;

void setSpeedStraight(float s) {
    speedStraight = s;
    pidLB.errint = 0;
    pidLF.errint = 0;
    pidRF.errint = 0;
    pidRB.errint = 0;
}

/**
 * @brief 各轮速度获取回调函数
 */
void CTRL_Callback(void) {
    static bool flag = true;
    if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
        setSpeedStraight(0);
        flag = true;
    } else if (flag) {
        setSpeedStraight(20);
        flag = false;
    }
    pidLB.goalstate = speedStraight;
    pidLF.goalstate = speedStraight;
    pidRF.goalstate = speedStraight;
    pidRB.goalstate = speedStraight;
}
