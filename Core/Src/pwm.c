#include "pwm.h"
#include "ctrl.h"
#include "tim.h"

volatile PIDTypeDef pidLB = { 0.0120, 4.2, 0.010, 0 };
volatile PIDTypeDef pidLF = { 0.012, 4, 0, 0 };
volatile PIDTypeDef pidRF = { 0.010, 2, 0.0, 0 };
volatile PIDTypeDef pidRB = { 0.0125, 3.9, 0.014, 0 };

#define EnableLB
#define EnableLF
#define EnableRF
#define EnableRB

/**
 * @brief 计算速度
 */
inline float calcWheelSpeed(TIM_HandleTypeDef* htim, float CNT2SP) {
    return (float)((int16_t)__HAL_TIM_GET_COUNTER(htim)) * CNT2SP;
}

/**
 * @brief 计算输出PWM占空比
 * @return float PWM占空比 [MINPWM, MAXPWM]
 */
inline float calcPWM(float newstate, volatile PIDTypeDef* instance) {
    float err = instance->goalstate - newstate;
    float olderr = instance->goalstate - instance->realstate;
    instance->errint += err * PIDPeriod / UnitFreq;
    float diff = (err - olderr) / PIDPeriod * UnitFreq;
    float pid = instance->Kp * (err + instance->Ki * instance->errint + instance->Kd * diff);
    return constrain(pid, MINPWM, MAXPWM);
}

/**
 * @brief 实际输出PWM占空比
 */
inline void outWheelPWM(float pwm, uint32_t Channel, GPIO_TypeDef* P_GPIOx, uint16_t P_GPIO_Pin, GPIO_TypeDef* N_GPIOx, uint16_t N_GPIO_Pin) {
    if (pwm >= 0) {
        HAL_GPIO_WritePin(P_GPIOx, P_GPIO_Pin, SET);
        HAL_GPIO_WritePin(N_GPIOx, N_GPIO_Pin, RESET);
        __HAL_TIM_SET_COMPARE(&TIM_PWM, Channel, (uint16_t)(pwm * PWMPeriod));
    } else {
        HAL_GPIO_WritePin(P_GPIOx, P_GPIO_Pin, RESET);
        HAL_GPIO_WritePin(N_GPIOx, N_GPIO_Pin, SET);
        __HAL_TIM_SET_COMPARE(&TIM_PWM, Channel, (uint16_t)(-pwm * PWMPeriod));
    }
}

/**
 * @brief pwm初始化
 */
void pwm_init(void) {
    // 使能PID中断
    HAL_TIM_Base_Start_IT(&TIM_PID);

    // 使能PWM输出
    HAL_TIM_PWM_Start(&TIM_PWM, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM_PWM, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&TIM_PWM, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&TIM_PWM, TIM_CHANNEL_4);

    // 使能编码器
    HAL_TIM_Encoder_Start(&TIM_LB_SP, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&TIM_LF_SP, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&TIM_RF_SP, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&TIM_RB_SP, TIM_CHANNEL_ALL);
}

/**
 * @brief PID定时器回调函数
 */
void TIM_PID_Callback(void) {
    CTRL_Callback();

    // LB
    {
        float newstate = calcWheelSpeed(&TIM_LB_SP, CNT2SP_LB);
        pidLB.pwm = calcPWM(newstate, &pidLB);
        pidLB.realstate = newstate;
        __HAL_TIM_SET_COUNTER(&TIM_LB_SP, 0);
    }

    // LF
    {
        float newstate = calcWheelSpeed(&TIM_LF_SP, CNT2SP_LF);
        pidLF.pwm = calcPWM(newstate, &pidLF);
        pidLF.realstate = newstate;
        __HAL_TIM_SET_COUNTER(&TIM_LF_SP, 0);
    }

    // RF
    {
        float newstate = calcWheelSpeed(&TIM_RF_SP, CNT2SP_RF);
        pidRF.pwm = calcPWM(newstate, &pidRF);
        pidRF.realstate = newstate;
        __HAL_TIM_SET_COUNTER(&TIM_RF_SP, 0);
    }

    // RB
    {
        float newstate = calcWheelSpeed(&TIM_RB_SP, CNT2SP_RB);
        pidRB.pwm = calcPWM(newstate, &pidRB);
        pidRB.realstate = newstate;
        __HAL_TIM_SET_COUNTER(&TIM_RB_SP, 0);
    }

#ifdef EnableLB
    outWheelPWM(pidLB.pwm, TIM_CH_LB_PWM, pinLBdirP_GPIO_Port, pinLBdirP_Pin, pinLBdirN_GPIO_Port, pinLBdirN_Pin);
#endif
#ifdef EnableLF
    outWheelPWM(pidLF.pwm, TIM_CH_LF_PWM, pinLFdirP_GPIO_Port, pinLFdirP_Pin, pinLFdirN_GPIO_Port, pinLFdirN_Pin);
#endif
#ifdef EnableRF
    outWheelPWM(pidRF.pwm, TIM_CH_RF_PWM, pinRFdirP_GPIO_Port, pinRFdirP_Pin, pinRFdirN_GPIO_Port, pinRFdirN_Pin);
#endif
#ifdef EnableRB
    outWheelPWM(pidRB.pwm, TIM_CH_RB_PWM, pinRBdirP_GPIO_Port, pinRBdirP_Pin, pinRBdirN_GPIO_Port, pinRBdirN_Pin);
#endif
}