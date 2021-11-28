#include "round2.h"
#include "strategy.h"
#include "zigbee.h"
#include "value.h"
#include "ctrl.h"
#include "delay.h"
#include "plan.h"

extern uint8_t maintestcount;

/**
 * @brief 信标初始化
 */
void initBeacon2()
{
    for (int32_t i = 0; i < 3; ++i)
    {
        beacon[i].x = getMyBeaconPosX(i), beacon[i].y = getMyBeaconPosY(i);
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
    {

        HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
        setSpeedStraight0();

        initBeacon2(); //初始化信标位置

        initWareHouse(); //初始化仓库类型

        measureMetalPos0(); //测量金属位置

        // int32_t pointNo = 0;    //点的序号
        // int32_t whouseNo = wareHouse[1].type;   //信标的仓库类型

        while (isEnable())
        {

            go2Point(getBestPoint());

            if (!isEnable())
            {
                return;
            }

            if (getGameTime() > 100)
            {
                send2WareHouseLAZY();
            }

            measureMetalPos();

            if(getCarMineSumNum() == 10)
            {
                send2WareHouseLAZY();
            }
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

// Pos findWHouse(int8_t )

int16_t getCarMineNumByType(int16_t type)
{
    if (type == 0)
        return getCarMineANum();
    else if (type == 1)
        return getCarMineBNum();
    else if (type == 2)
        return getCarMineCNum();
    else if (type == 3)
        return getCarMineDNum();
    else
    {
        return 0;
    }
}

bool waitUtilNumChange()
{
    //破釜沉舟的时候把它去掉
    int16_t thisNum = getCarMineSumNum();
    int16_t lastNum = getCarMineSumNum();
    while (thisNum == lastNum)
    {
        // lastNum = thisNum;
        thisNum = getCarMineSumNum();
        if (!isEnable())
        {
            return false;
        }
    }
    return true;
}

void send2WareHouseLAZY()
{
    int16_t numToLoad = 3;
    if (getCarMineNumByType(wareHouse[1].type) >= numToLoad || getGameTime() > 100)
    {
        Pos dest = {wareHouse[1].x, wareHouse[1].y};
        go2Point(dest);
        waitUtilNumChange();
        if (!isEnable())
        {
            return;
        }
    }

    for (int16_t i = 0; i < 3; i++)
    {
        if (getCarMineNumByType(beacon[i].type) >= numToLoad || getGameTime() > 100)
        {
            Pos dest = {beacon[i].x, beacon[i].y};
            go2Point(dest);
            waitUtilNumChange();
            if (!isEnable())
            {
                return;
            }
        }
    }
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
    carPos = getCarPosByRound();
    int32_t time0 = getCar2PointTime(metal[0]);
    int32_t time1 = getCar2PointTime(metal[1]);
    Pos metalPos[2] = {{metal[0].x, metal[0].y}, {metal[1].x, metal[1].y}};
    return (time0 < time1) ? metalPos[0] : metalPos[1];
}