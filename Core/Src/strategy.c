#include"strategy.h"
#include"ctrl.h"
#include<math.h>
#include"uppercom.h"
#include"delay.h"


void go2Point(Pos dest)
{
    // Pos curPos = getCurPos();
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