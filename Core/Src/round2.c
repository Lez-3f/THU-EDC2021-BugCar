#include"round2.h"
#include"strategy.h"
#include"zigbee.h"
#include"value.h"
#include"ctrl.h"
#include"delay.h"
#include"plan.h"

extern uint8_t maintestcount;

/**
 * @brief 信标初始化
 */
void initBeacon2()
{
    for (int32_t i = 0; i < 3; ++i)
    {
        beacon[0].x = getMyBeaconPosX(i), beacon[1].y = getMyBeaconPosY(i);
    }
}

void loop2()
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
    {/*
        HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
        setSpeedStraight0();

        initBeacon2(); //初始化信标位置

        initWareHouse();    //初始化仓库类型

        measureMetalPos0(ROUND_2); //测量金属位置

        // int32_t pointNo = 0;    //点的序号
        // int32_t whouseNo = wareHouse[1].type;   //信标的仓库类型

        go2Point(getBestPoint());

        while(isEnable()){

            measureMetalPos(ROUND_2);

            Pos dest = getBestPoint();

            if(getGameTime() > 100)
            {
                send2WareHouse();
            }
        }
*/
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

void send2WareHouse()
{
    Object points[4];
    Object b_p[4];
}

int32_t getCar2PointTime(Object point)
{
    float car_angle = getRealAngle();
    int32_t vec[2] = {point.x - carPos.x, point.y - carPos.y};
    float distance = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
    float dest_angle = atan2(vec[1], vec[0]);
    float relaAngle = ANGLE_NORM(dest_angle - car_angle);
    relaAngle = 90 - fabs(90 - fabs(relaAngle));
    return T_straight * distance + T_trun * relaAngle;
}

Pos getBestPoint()
{
    carPos = getCarPos();
    int32_t time0 = getCar2PointTime(metal[0]);
    int32_t time1 = getCar2PointTime(metal[1]);
    Pos metalPos[2] = {{metal[0].x, metal[0].y}, {metal[1].x, metal[1].y}};
    return (time0 > time1) ? metalPos[0] : metalPos[1];
}