#include"round1.h"
#include"strategy.h"
#include"value.h"
#include"ctrl.h"
#include"delay.h"
#include"plan.h"
#include"zigbee.h"

#define TRI_D_M 15  //测量金属所用的三角形边长
#define X_B 132
#define Y_B 129
#define TRI_D_B 40  //测量小车位置所用的信标三角形边长

extern uint8_t maintestcount;

extern Object metal[0] = {0, 0, 'A'},
metal[1] = {0, 0, 'A'},
beacon[0] = {0, 0, 'N'},
beacon[1] = {0, 0, 'N'},
beacon[2] = {0, 0, 'N'};

/**
 * @brief 信标初始化
 */
void initBeacon()
{
	beacon[0].x = X_B - TRI_D_B, beacon[0].y = Y_B;
	beacon[1].x = X_B + TRI_D_B / 2, beacon[1].y = Y_B - TRI_D_B * 0.852;
	beacon[2].x = X_B + TRI_D_B / 2, beacon[2].y = Y_B + TRI_D_B * 0.852;
}

/* 上半回合 */
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

        measureMetalPos(); //测量金属位置

        Object points[6];
        points[0] = metal[0], points[1] = metal[1];
        for (int i = 2; i < 4; ++i)
        {
            points[i] = beacon[i - 2];
        }

        Object b_p[6];
        // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_SET);
        getBestPlan(points, b_p);
        // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
        // //测试
        // delay_ms(1000);

        int PointNo = 0;    //点的序号
        int whouseNo = wareHouse[1].type;   //信标的仓库类型

        while(isEnable() && PointNo < 6)
        {
            Pos dest;
            dest.x = b_p[PointNo].x, dest.y = b_p[PointNo].y;
            go2Point(dest);
            if(b_p[PointNo].type == 'N')    //信标
            {
                zigbeeSend((++whouseNo) % 4);
                HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_SET);  //点亮小灯
                delay_ms(1000);
                HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);  
            }
            delay_ms(100);
            ++PointNo;
        }

        // int i = 0;
        // for (i = 0; i < 500; ++i)
        // {
        //     if(isEnable()){
        //         delay_ms(10);
        //     }
        // }
    }
    delay_ms(10);
}

void measureMetalPos()
{
    Pos mPoints[3]; //三个测量点
    uint32_t mIntensity[2][3];  //三个强度值

    mPoints[0] = getCarPos();
    mIntensity[0][0] = getMineArrayType(0);
    mIntensity[1][0] = getMineArrayType(1);

    Pos dest1;
    dest1.x = mPoints[0].x - TRI_D_M, dest1.y = mPoints[0].y;
    go2Point(dest1);

    mPoints[1] = getCarPos();
    mIntensity[0][1] = getMineArrayType(0);
    mIntensity[1][1] = getMineArrayType(1);

    Pos dest2;
    dest2.x = mPoints[1].x + TRI_D_M / 2, dest2.y = mPoints[1].y + TRI_D_M * 1.7;
    go2Point(dest2);

    mPoints[2] = getCarPos();
    mIntensity[0][2] = getMineArrayType(0);
    mIntensity[1][2] = getMineArrayType(1);

    Pos m1 = calMetPos(mIntensity[0], mPoints);
    Pos m2 = calMetPos(mIntensity[1], mPoints);

    metal[0].x = m1.x, metal[0].y = m1.y;
    metal[1].x = m2.x, metal[1].y = m2.y;

}