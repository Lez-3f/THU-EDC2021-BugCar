#include "strategy.h"
#include "ctrl.h"
#include <math.h>
#include <stdlib.h>
#include "delay.h"
#include "pwm.h"
#include "value.h"
#include "zigbee.h"

#define PW2(X) ((X) * (X))
#define A 1e9

int32_t metalNum = 0;

Object wareHouse[8] = {{15, 15}, {127, 15}, {239, 15}, {239, 127}, {239, 239}, {127, 239}, {15, 239}, {15, 127}}; // warehouse
Object metal[2] = {{10, 10, 'A'}, {11, 11, 'A'}};
Object beacon[3] = {
    {0, 0, 'N'},
    {0, 0, 'N'},
    {0, 0, 'N'}};

Pos carPos = {0, 0};

Pos CurrDest;

bool isEnable()
{
    return getGameState() == 1 && HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) == GPIO_PIN_RESET;
}

void initWareHouse()
{
    for (int32_t i = 0; i < 8; ++i)
    {
        wareHouse[i].type = getParkDotMineType(i);
    }
}

void measureMetalPos0()
{
    Pos mPoints[3];           //三个测量点
    int32_t mIntensity[2][3]; //三个强度值

    mPoints[0] = getCarPosByRound();
    mIntensity[0][0] = getMineIntensity(0);
    mIntensity[1][0] = getMineIntensity(1);

    Pos dest1;
    dest1.x = mPoints[0].x + TRI_D_M, dest1.y = mPoints[0].y;
    // dest1.x = mPoints[0].x - TRI_D_M * 0.5, dest1.y = mPoints[0].y + TRI_D_M * 0.866;
    go2Point(dest1);

    mPoints[1] = getCarPosByRound();
    mIntensity[0][1] = getMineIntensity(0);
    mIntensity[1][1] = getMineIntensity(1);

    Pos dest2;
    dest2.x = mPoints[0].x + TRI_D_M * 0.5, dest2.y = mPoints[0].y + TRI_D_M * 0.866;
    // dest2.x = mPoints[0].x, dest2.y = mPoints[0].y + TRI_D_M * 0.866;
    go2Point(dest2);

    mPoints[2] = getCarPosByRound();
    mIntensity[0][2] = getMineIntensity(0);
    mIntensity[1][2] = getMineIntensity(1);

    Pos m1 = calMetPos(mIntensity[0], mPoints);
    Pos m2 = calMetPos(mIntensity[1], mPoints);

    metal[0].x = m1.x, metal[0].y = m1.y;
    metal[1].x = m2.x, metal[1].y = m2.y;
}

Pos getCarPosByRound()
{
    if (getIsCarPosValid())
    {
        return getCarPos();
    }
    else
    {
        return getCarPosByBeacon();
    }
}

void measureMetalPos()
{
    Pos mPoints[3];           //三个测量点
    int32_t mIntensity[2][3]; //三个强度值
    float angle0 = getRealAngle();

    mPoints[0] = getCarPosByBeacon();
    mIntensity[0][0] = getMineIntensity(0);
    mIntensity[1][0] = getMineIntensity(1);

    move(angle0, (float)TRI_D_M, (float)SPEED);

    mPoints[1] = getCarPosByBeacon();
    mIntensity[0][1] = getMineIntensity(0);
    mIntensity[1][1] = getMineIntensity(1);

    move(angle0 + 60, (float)TRI_D_M, (float)(-SPEED));

    mPoints[2] = getCarPosByBeacon();
    mIntensity[0][2] = getMineIntensity(0);
    mIntensity[1][2] = getMineIntensity(1);

    Pos m1 = calMetPos(mIntensity[0], mPoints);
    Pos m2 = calMetPos(mIntensity[1], mPoints);

    metal[0].x = m1.x, metal[0].y = m1.y;
    metal[1].x = m2.x, metal[1].y = m2.y;
}

bool turnAngle(float angle)
{
    setAngle(angle);
    while (!isAngleCompleted())
    {
        if (!isEnable())
        {
            setSpeedStraight0();
            setAngle(getRealAngle());
            return false;
        }
        delay_ms(10);
    } //转角度

    return true;
}

bool goStraight(float speed, float distance)
{
    setSpeedStraight(speed, distance);
    while (!isDisCompleted())
    {
        if (!isEnable())
        {
            setSpeedStraight0();
            setAngle(getRealAngle());
            return false;
        }
        delay_ms(10);
    }

    return true;
}

bool move(float angle, float distance, float speed)
{
    setAngle(angle);
    while (!isAngleCompleted())
    {
        if (!isEnable())
        {
            setSpeedStraight0();
            setAngle(getRealAngle());
            return false;
        }
        delay_ms(10);
    } //转角度

    setSpeedStraight(speed, distance);

    while (!isDisCompleted())
    {
        if (!isEnable())
        {
            setSpeedStraight0();
            setAngle(getRealAngle());
            return false;
        }
        delay_ms(10);
    }

    return true;
}

char checkTypeByPos(Pos dest)
{
    for (int i = 0; i < 2; ++i)
    {
        if (dest.x == metal[i].x && dest.y == metal[i].y)
        {
            return 'm';
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        if (dest.x == beacon[i].x && dest.y == beacon[i].y)
        {
            return 'b';
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        if (dest.x == wareHouse[i].x && dest.y == wareHouse[i].y)
        {
            return 'w';
        }
    }
    return 'p';
}

void go2Point(Pos dest)
{
    if(dest.x <5 || dest.y < 5 || dest.x>250||dest.y>250){
        dest.x = 127;
        dest.y  = 127;
    }
    CurrDest = dest;
    char type = checkTypeByPos(dest);
    metalNum = getCarMineSumNum();

    //先转一个角度
    Pos curPos = getCarPosByRound();
    Pos relaPos = {dest.x - curPos.x, dest.y - curPos.y};

    float angle0 = getRealAngle();                              //获取当前车身角度
    double relaAngle = -180 * atan2(relaPos.y, relaPos.x) / PI; //计算位移角度
    bool isBackWards = fabs(ANGLE_NORM(relaAngle - angle0)) > 90;

    // float distance = sqrt(PW2(dest.x - curPos.x) + PW2(dest.y - curPos.y)); //小车与目标点的距离
    float angleturn = (isBackWards) ? 180 + relaAngle : relaAngle;
    // float speed = (isBackWards) ? -SPEED : SPEED;

    // if (!turnAngle(angleturn))
    // {
    //     return;
    // }

    // //转过去后再次计算
    // curPos = getCarPosByRound();
    // relaPos.x = dest.x - curPos.x, relaPos.y = dest.y - curPos.y;

    // angle0 = getRealAngle();                             //获取当前车身角度
    // relaAngle = -180 * atan2(relaPos.y, relaPos.x) / PI; //计算位移角度
    // isBackWards = fabs(ANGLE_NORM(relaAngle - angle0)) > 90;

    float distance = REV_PARAM * sqrt(PW2(dest.x - curPos.x) + PW2(dest.y - curPos.y)); //小车与目标点的距离
    // angleturn = (isBackWards) ? 180 + relaAngle : relaAngle;
    float speed = (isBackWards) ? -SPEED : SPEED;
    if (distance < 50 && type == 'w')
    {
        speed = (isBackWards) ? -SPEED_SLOW: SPEED_SLOW;
    }

    if (!move(angleturn, distance, speed))
    {
        return;
    }

    // setSpeedStraight(speed, distance);

    // while (isEnable())
    // {
    //     curPos = getCarPosByRound(); //更新当前坐标
    //     relaPos.x = dest.x - curPos.x;
    //     relaPos.y = dest.y - curPos.y;
    //     distance = sqrt(PW2(dest.x - curPos.x) + PW2(dest.y - curPos.y));

    //     angle0 = getRealAngle(); //更新当前车身角度

    //     relaAngle = -180 * atan2(relaPos.y, relaPos.x) / PI; //计算位移角度
    //     isBackWards = fabs(ANGLE_NORM(relaAngle - angle0)) > 90;
    //     angleturn = (isBackWards) ? 180 + relaAngle : relaAngle;

    //     setAngle(angleturn);

    //     if (distance < 30)
    //     {
    //         speed = (isBackWards) ? -SPEED_SLOW : SPEED_SLOW;
    //         setSpeedStraight(speed, distance);
    //         break;
    //     }

    //     delay_ms(150);
    // }

    //慢速下再次设置
    // delay_ms(150);               //保证接收到一次慢速下坐标

    //到达目标点后检测，
    curPos = getCarPosByRound();
    relaPos.x = dest.x - curPos.x;
    relaPos.y = dest.y - curPos.y;
    distance = REV_PARAM * sqrt(PW2(dest.x - curPos.x) + PW2(dest.y - curPos.y));

    while (distance > 5)
    {
        //金属 数量变化
        if (type == 'm')
        {
            if (getCarMineSumNum() > metalNum)
            {
                break;
            }
        }
        //信标 1-退出， 2-继续
        if (type == 'b')
        {
            if (getCarTask() == ROUND_1)
            {
                break;
            }
        }

        //仓库 继续

        // 测量点 退出
        if (type == 'p')
        {
            break;
        }

        angle0 = getRealAngle();                             //更新当前车身角度
        relaAngle = -180 * atan2(relaPos.y, relaPos.x) / PI; //计算位移角度
        isBackWards = fabs(ANGLE_NORM(relaAngle - angle0)) > 90;
        angleturn = (isBackWards) ? 180 + relaAngle : relaAngle;
        speed = (isBackWards) ? -SPEED_SLOW : SPEED_SLOW;
        if (!move(angleturn, distance, speed))
        {
            return;
        }
        curPos = getCarPosByRound(); //更新当前坐标
        relaPos.x = dest.x - curPos.x;
        relaPos.y = dest.y - curPos.y;
        distance = REV_PARAM * sqrt(PW2(dest.x - curPos.x) + PW2(dest.y - curPos.y));
    }

    // setSpeedStraight0();
    // setAngle(getRealAngle());
    // delay_ms(2000);  //太慢了
}

Pos calMetPos(int32_t S[3], Pos P[3])
{
    // for(int i = 0; i < 3; ++i)
    //     S[i] = 1000000;
    // P[0].x = 0, P[0].y = 0;
    // P[1].x = 30, P[1].y = 0;
    // P[2].x = 30, P[2].y = 30;
    int32_t aa[2], bb[2], cc[2], AA, BB, CC, x0, y0;
    int32_t i = 0;
    for (i = 0; i < 2; i++)
    {
        aa[i] = 2 * P[i + 1].x - 2 * P[i].x;
        bb[i] = 2 * P[i + 1].y - 2 * P[i].y;
        cc[i] = A / S[i] - A / S[i + 1] + PW2(P[i + 1].x) - PW2(P[i].x) + PW2(P[i + 1].y) - PW2(P[i].y);
    }
    AA = aa[0] * bb[1] - aa[1] * bb[0];
    BB = aa[0] * cc[1] - aa[1] * cc[0];
    CC = bb[0] * cc[1] - bb[1] * cc[0];
    x0 = -CC / AA;
    y0 = BB / AA;
    Pos p;
    p.x = x0, p.y = y0;
    return p;
}

Pos getCarPosByBeacon()
{
    int32_t d[3];
    Pos b[3];
    for (int i = 0; i < 3; ++i)
    {
        d[i] = getDistanceOfMyBeacon(i);
        b[i].x = beacon[i].x, b[i].y = beacon[i].y;
    }
    return calCarPosByBeacon(d, b);
}

Pos calCarPosByBeacon(int32_t d[3], Pos b[3])
{
    int32_t aa[2], bb[2], cc[2], AA, BB, CC, x0, y0;
    int32_t i = 0;
    for (i = 0; i < 2; ++i)
    {
        aa[i] = 2 * b[i].x - 2 * b[i + 1].x;
        bb[i] = 2 * b[i].y - 2 * b[i + 1].y;
        cc[i] = PW2(d[i + 1]) - PW2(d[i]) + PW2(b[i].x) + PW2(b[i].y) - PW2(b[i + 1].x) - PW2(b[i + 1].y);
    }
    AA = aa[0] * bb[1] - aa[1] * bb[0];
    BB = aa[0] * cc[1] - aa[1] * cc[0];
    CC = bb[0] * cc[1] - bb[1] * cc[0];
    x0 = -CC / AA;
    y0 = BB / AA;
    Pos p = {x0, y0};
    return p;
}