#include "round1.h"

/* 上半回合 */
void loop1()
{
    if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET)
    {
        setSpeedStraight0();
        setAngle(getRealAngle());
    }
    else
    {
        setSpeedStraight0();
        // 转弯前角度
        float angle0 = getRealAngle();
        setAngle(angle0);
        for (int i = 0; i < 4; ++i)
        {
            setSpeedStraight(40, 30);
            while (!isDisCompleted())
            {
                if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET)
                {
                    return;
                }
            }
            setAngle(angle0 - 90 * (i + 1));
            while (!isAngleCompleted())
            {
                if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET)
                {
                    return;
                }
            }
        }

        // 防过热暂停
        delay_ms(1000);

        for (int i = 0; i < 4; ++i)
        {
            setAngle(angle0 + 90 * (i + 1));
            while (!isAngleCompleted())
            {
                if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET)
                {
                    return;
                }
            }
            setSpeedStraight(-40, 30);
            while (!isDisCompleted())
            {
                if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET)
                {
                    return;
                }
            }
        }
    }
    delay_ms(20);
}