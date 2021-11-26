#include"strategy.h"
#include"ctrl.h"
#include<math.h>
#include<stdlib.h>
#include"uppercom.h"
#include"delay.h"
#include"pwm.h"
#include"value.h"

#define PW2(X) ((X)*(X))
#define A 1e8

Object wareHouse[8] = {{0, 0}, {0, 127}, {0, 254}, {127, 0}, {127, 254}, {254, 0}, {254, 127}, {254, 254}}; // warehouse

bool isEnable()
{
    return !isGamePause() && HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) == GPIO_PIN_RESET;
}

void go2Point(Pos dest)
{
    Pos curPos = getCurPos();

    Pos relaPos;
    relaPos.x = dest.x - curPos.x;
    relaPos.y = dest.y - curPos.y;

    float angle0 = getRealAngle();  //获取当前车身角度

    double relaAngle = - 180 * atan2(relaPos.x, relaPos.y) / PI; //计算位移角度
    float distance = sqrt(PW2(relaPos.x) + PW2(relaPos.y)); //小车的距离

    float difAngle = ANGLE_NORM(relaAngle - angle0);

    if(fabs(difAngle) > 90)
    {
        setAngle(180+relaAngle);
        while(!isAngleCompleted())
        {
            if(!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }   //转角度

        setSpeedStraight(-SPEED, distance);

        while(!isDisCompleted())
        {
            if (!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }
    }

    else{
        setAngle(relaAngle);
        while(!isAngleCompleted())
        {
            if(!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }   //转角度

        setSpeedStraight(SPEED, distance);

        while(!isDisCompleted())
        {
            if (!isEnable())
            {
                setSpeedStraight0();
                setAngle(getRealAngle());
                return;
            }
            delay_ms(10);
        }
    }
}


Pos calMetPos(int S[3], Pos P[3])
{
	int aa[2], bb[2], cc[2], AA, BB, CC, x0, y0;
	int i = 0;
	for (i = 0; i < 2; i++)
	{
		aa[i] = 2 * P[i + 1].x - 2 * P[i].x;
		bb[i] = 2 * P[i + 1].y - 2 * P[i].y;
		cc[i] = A / S[i] - A / S[i + 1]+PW2(P[i+1].x)-PW2(P[i].x)+PW2(P[i+1].y)-PW2(P[i].y);
	}
	AA = aa[0] * bb[1] - aa[1] * bb[0];
	BB = aa[0] * cc[1] - aa[1] * cc[0];
	CC = bb[0] * cc[1] - bb[1] * cc[0];
	x0 = -CC / AA;
	y0 = BB / AA;
    Pos p = {x0, y0};
    return p;
}

Pos calCarPosByBeacon(int d[3])
{
	int aa[2], bb[2], cc[2], AA, BB, CC, x0, y0;
	int i = 0;
	for (i = 0; i < 2; ++i)
	{
        aa[i] = 2 * b[i].x - 2 * b[i + 1].x;
        bb[i] = 2 * b[i].y - 2 * b[i + 1].y;
        cc[i] = PW2(d[i + 1]) - PW2(d[i]) + PW2(b[i].x) + PW2(b[i].y) - PW2(b[i+1].x) - PW2(b[i+1].y);
    }
    AA = aa[0] * bb[1] - aa[1] * bb[0];
	BB = aa[0] * cc[1] - aa[1] * cc[0];
	CC = bb[0] * cc[1] - bb[1] * cc[0];
    x0 = -CC / AA;
    y0 = BB / AA;
    Pos p = {x0, y0};
    return p;
}