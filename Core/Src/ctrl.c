#include "ctrl.h"
#include "pwm.h"
#include "gyro.h"
#include "usart.h"
#include "main.h"

volatile float speedStraight = 0.0;
volatile bool angleCompleted = false;

uint8_t testDataBuf[128];

/**
 * @brief PID计算前回调函数
 */
void CTRL_Callback(void) {
    {
        float newstate = gyroAngle.z;
        float err = pidAngle.goalstate - newstate;
        err = ANGLE_NORM(err);
        if (err < ANGLE_COMPLETE_DELTA && err > -ANGLE_COMPLETE_DELTA) {
            angleCompleted = true;
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

/**
 * @brief PID计算后回调函数
 */
void CTRL_After_Callback(void) {
    // uprintf(&UART_COMM, "%f %f %f %f ", pidLB.realstate, pidLB.pwm, pidLF.realstate, pidLF.pwm);
    // uprintf(&UART_COMM, "%f %f %f %f \n", pidRF.realstate, pidRF.pwm, pidRB.realstate, pidRB.pwm);

    // uprintf(&UART_COMM, "%d %d %d %d \n", (int16_t)__HAL_TIM_GET_COUNTER(&TIM_LB_SP), (int16_t)__HAL_TIM_GET_COUNTER(&TIM_LF_SP),
      // (int16_t)__HAL_TIM_GET_COUNTER(&TIM_RF_SP), (int16_t)__HAL_TIM_GET_COUNTER(&TIM_RB_SP));

    uprintf_DMA(testDataBuf, &UART_COMM, "%f %f %f %f %f \n", pidLB.realstate, pidLF.realstate, pidRF.realstate, pidRB.realstate, getRealAngle());
}

/**
 * @brief 获取设定的直行速度
 * @return float 设定直行速度
 */
float getSpeedStraight() {
    return speedStraight;
}

/**
 * @brief 获取实际的直行速度
 * @return float 实际直行速度
 */
float getRealSpeedStraight() {
    float sum = 0;
    sum += pidLB.realstate;
    sum += pidLF.realstate;
    sum += pidRF.realstate;
    sum += pidRB.realstate;
    return sum / 4;
}

/**
 * @brief 设定直行速度
 * @param s 直行速度
 */
void setSpeedStraight(float s) {
    speedStraight = s;
    pidLB.errint = 0;
    pidLF.errint = 0;
    pidRF.errint = 0;
    pidRB.errint = 0;
}

/**
 * @brief 获取设定的修正绝对角度
 * @return float 设定修正绝对角度[-180,180]
 */
float getAngle() {
    return ANGLE_NORM(pidAngle.goalstate + GYRO_REVISE_BASE);
}

/**
 * @brief 获取实际的修正绝对角度
 * @return float 实际修正绝对角度[-180,180]
 */
float getRealAngle() {
    return ANGLE_NORM(pidAngle.realstate + GYRO_REVISE_BASE);
}

/**
 * @brief 设定修正绝对角度
 * @param ag 修正绝对角度
 */
void setAngle(float ag) {
    pidAngle.goalstate = ANGLE_NORM(ag - GYRO_REVISE_BASE);
    pidAngle.errint = 0;
    angleCompleted = false;
}

/**
 * @brief 获取转动完成状态
 * @return true 转动完成(误差<ANGLE_COMPLETE_DELTA); false 转动未完成
 */
bool isAngleCompleted() {
    return angleCompleted;
}