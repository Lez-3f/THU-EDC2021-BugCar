#include"round1.h"
#include"strategy.h"
#include"value.h"
#include"ctrl.h"
#include"delay.h"
#include"plan.h"

extern uint8_t maintestcount;

Pos R1_Points[6] = {{0, 0}};


/* 上半回合 */
void loop1()
{
    // Object points[5] = {{120, 120, 'A'}, {55, 55, 'A'}, {127, 112, 'A'}, {127, 127, 'A'}, {127, 142, 'A'}};
    // Pos b_p[6];
    // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_SET);
    // getBestPlan(points, b_p);
    // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
    // delay_ms(3000);

    //test
    if (!isEnable())
    {
        ++maintestcount;
        if (maintestcount >= 5)
        {
            maintestcount -= 5;
            HAL_GPIO_TogglePin(pinLED_GPIO_Port, pinLED_Pin);
        }
        setSpeedStraight0();
        setAngle(getRealAngle());
    }
    else
    {
        HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);

        setSpeedStraight0();
        Pos dest = {50, 50};
        go2Point(dest);

        dest.x = -50, dest.y = -50;
        go2Point(dest);

        int i = 0;
        for (i = 0; i < 500; ++i)
        {
            if(isEnable()){
                delay_ms(10);
            }
        }
    }
    delay_ms(10);
}