#include "ctrl.h"
#include "pwm.h"
#include "gyro.h"
#include "usart.h"
#include "main.h"
#include "tim.h"

volatile float speedStraight = 0.0;
volatile int32_t cnt4Goal = 0;
volatile int32_t cnt4Real = 0;
volatile bool disCompleted = true;
volatile bool angleCompleted = false;
bool angleCompletedLastflag = false;

uint8_t testCount = 0;
#pragma pack(1)
struct {
    uint8_t head;
    float LB;
    float LF;
    float RF;
    float RB;
    float angle;
    float dis;
} testDataBuf;
#pragma pack()

/**
 * @brief PID计算前回调函数
 */
void CTRL_Callback(void) {
    {
        float newstate = gyroAngle.z;
        float err = pidAngle.goalstate - newstate;
        err = ANGLE_NORM(err);
        if (err < ANGLE_COMPLETE_DELTA && err > -ANGLE_COMPLETE_DELTA) {
            angleCompleted = angleCompletedLastflag;
            angleCompletedLastflag = true;
        }

        pidAngle.pwm = calcAnglePWM(err, &pidAngle);
        if (err > ANGLE_QUICK_DELTA && err < -ANGLE_QUICK_DELTA) {
            pidAngle.pwm = err > 0 ? 1 : -1;
        }

        pidAngle.realstate = newstate;
    }

    if (!disCompleted) {
        cnt4Real += sgn(speedStraight) *
            ((int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&TIM_LB_SP)
                + (int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&TIM_LF_SP)
                + (int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&TIM_RF_SP)
                + (int32_t)(int16_t)__HAL_TIM_GET_COUNTER(&TIM_RB_SP));
        if (cnt4Real >= cnt4Goal) {
            disCompleted = true;
            setSpeedStraight0();
        }
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
    ++testCount;
    if (testCount >= 1) {
        testCount = 0;
        // uprintf(&UART_COMM, "%f %f %f %f ", pidLB.realstate, pidLB.pwm, pidLF.realstate, pidLF.pwm);
        // uprintf(&UART_COMM, "%f %f %f %f \n", pidRF.realstate, pidRF.pwm, pidRB.realstate, pidRB.pwm);

        // uprintf(&UART_COMM, "%d %d %d %d \n", (int16_t)__HAL_TIM_GET_COUNTER(&TIM_LB_SP), (int16_t)__HAL_TIM_GET_COUNTER(&TIM_LF_SP),
            // (int16_t)__HAL_TIM_GET_COUNTER(&TIM_RF_SP), (int16_t)__HAL_TIM_GET_COUNTER(&TIM_RB_SP));

        if (UART_COMM.gState == HAL_UART_STATE_READY) {
            testDataBuf.head = 0xAA;
            testDataBuf.LB = pidLB.realstate;
            testDataBuf.LF = pidLF.realstate;
            testDataBuf.RF = pidRF.realstate;
            testDataBuf.RB = pidRB.realstate;
            testDataBuf.angle = getRealAngle();
            testDataBuf.dis = cnt4Real / DIS2CNT4_AVE;
            uwrite_DMA(&UART_COMM, (uint8_t*)&testDataBuf, sizeof(testDataBuf));
            // uwrite_DMA(&UART_COMM, gyroMessage.buf, sizeof(gyroMessage));
        }

        // uprintf_DMA(testDataBuf, &UART_COMM, "%f %f %f %f %f \n", pidLB.realstate, pidLF.realstate, pidRF.realstate, pidRB.realstate, getRealAngle());
    }
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
 * @param dis 直行距离
 */
void setSpeedStraight(float s, float dis) {
    dis = constrain(dis, -MAX_DISTANCE, MAX_DISTANCE);
    if (dis >= 0) {
        speedStraight = s;
        cnt4Goal = (int32_t)round(dis * DIS2CNT4_AVE);
    } else {
        speedStraight = -s;
        cnt4Goal = (int32_t)round(-dis * DIS2CNT4_AVE);
    }

    cnt4Real = 0;
    disCompleted = false;
    pidLB.errint = 0;
    pidLF.errint = 0;
    pidRF.errint = 0;
    pidRB.errint = 0;
}

/**
 * @brief 设定速度为0
 */
void setSpeedStraight0() {
    speedStraight = 0;

    pidLB.errint = 0;
    pidLF.errint = 0;
    pidRF.errint = 0;
    pidRB.errint = 0;
}

/**
 * @brief 获取直行完成状态
 * @return true 完成(或略超出); false 未完成
 */
bool isDisCompleted() {
    return disCompleted;
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
    angleCompletedLastflag = false;
    angleCompleted = false;
}

/**
 * @brief 获取转动完成状态
 * @return true 转动完成(误差<ANGLE_COMPLETE_DELTA); false 转动未完成
 */
bool isAngleCompleted() {
    return angleCompleted;
}