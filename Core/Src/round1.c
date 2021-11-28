#include "round1.h"
#include "strategy.h"
#include "value.h"
#include "ctrl.h"
#include "delay.h"
#include "plan.h"
#include "zigbee.h"

extern uint8_t maintestcount;


/**
 * @brief 信标初始化
 */
void initBeacon()
{
    beacon[0].x = X_B - TRI_D_B, beacon[0].y = Y_B; //92,129
    beacon[1].x = X_B + TRI_D_B / 2, beacon[1].y = Y_B - TRI_D_B * 0.866;   //152,94
    beacon[2].x = X_B + TRI_D_B / 2, beacon[2].y = Y_B + TRI_D_B * 0.866;   //152,163
}

/**
 * @brief 上半回合
 */
void loop1()
{
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

        initBeacon(); //初始化信标位置

        initWareHouse(); //初始化仓库类型

        measureMetalPos0(); //测量金属位置

        Object points[5];
        points[0] = metal[0], points[1] = metal[1];
        for (int32_t i = 2; i < 5; ++i)
        {
            points[i] = beacon[i - 2];
        }

        Object b_p[6] = {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}};
        // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_SET);
        getBestPlan(points, b_p);
        // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
        // //测试
        // delay_ms(1000);

        int32_t PointNo = 0;                  //点的序号
        int32_t whouseNo = wareHouse[1].type; //信标的仓库类型

        while (isEnable() && PointNo < 6)
        {
            Pos dest;
            dest.x = b_p[PointNo].x, dest.y = b_p[PointNo].y;
            go2Point(dest);
            if (b_p[PointNo].type == 'N') //信标
            {
                zigbeeSend((++whouseNo) % 4);
                HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_SET);  //点亮小灯
                delay_ms(1000);
                HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
            }

            delay_ms(100);
            ++PointNo;
        }

        //比赛结束
        while (isEnable())
        {
            delay_ms(10);
        }

        // int32_t i = 0;
        // for (i = 0; i < 500; ++i)
        // {
        //     if(isEnable()){
        //         delay_ms(10);
        //     }
        // }
    }
    delay_ms(10);
}