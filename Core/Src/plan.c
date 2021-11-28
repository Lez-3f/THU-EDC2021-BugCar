#include "strategy.h"
#include <math.h>
#include "value.h"
#include "zigbee.h"
#include "ctrl.h"
#include "plan.h"

Pos curPos;

int32_t best_time;
int32_t best_iw;

float abs_arctan_dif(int32_t t1[2], int32_t t2[2])
{
    //返回0—180
    int32_t x = t1[1] * t2[1] + t1[0] * t2[0],
            y = t1[0] * t2[1] - t1[1] * t2[0];
    float ax, ay, t, r;
    ax = x > 0 ? x : -x;
    ay = y > 0 ? y : -y;
    if (ax > ay)
    {
        t = ay / ax;
        r = t * (45 - 15.66 * (t - 1));
    }
    else if (ay == 0)
        r = 0;
    else
    {
        t = ax / ay;
        r = 90 - t * (45 - 15.66 * (t - 1));
    }
    if (x < 0)
        r = 180 - r;
    // printf("%f\n",r);
    return r;
}

int32_t time_cpw(Object *points, int32_t i_w, int32_t num)
{
    // num个中间点，1个始car，1个末warehouse
    // Pos curPos = getCarPos();
    if (num > 5)
    {
        return 0;
    }
    int32_t vec[6][2];
    int32_t i = 0;
    vec[0][0] = points[0].x - curPos.x;
    vec[0][1] = points[0].y - curPos.y;
    for (i = 0; i < 4; i++)
    {
        vec[i+1][0] = points[i+1].x - points[i].x;
        vec[i+1][1] = points[i+1].y - points[i].y;
    }
    vec[5][0] = wareHouse[i_w].x - points[4].x;
    vec[5][1] = wareHouse[i_w].y - points[4].y;

    float car_angle = getRealAngle(), vec1_angle = atan2(vec[0][1], vec[0][0]) * 180 / 3.1415926;
    int32_t angle = ANGLE_NORM(car_angle - vec1_angle); //获取总角度,len=num+1
    angle = 90 - fabs(90 - fabs(angle));
    for (i = 1; i < num + 1; i++)
    {
        if (vec[i - 1][0] * vec[i][0] + vec[i - 1][1] * vec[i][1] > 0)
            angle += abs_arctan_dif(vec[i - 1], vec[i]);
        else
            angle += 180 - abs_arctan_dif(vec[i - 1], vec[i]);
        // printf("%d\n", angle);
    }

    int32_t distance = 0; //获取总路程,len=num+1
    for (i = 0; i < num + 1; i++)
        distance += sqrt(vec[i][0] * vec[i][0] + vec[i][1] * vec[i][1]);

    // int time = T_trun * angle + T_straight * distance;
    return T_trun * angle + T_straight * distance;
}

void permutation_cpw(int32_t k, int32_t num, Object *points, Object *b_p)
{
    //递归到底层
    if (k == num - 1)
    {
        int32_t i;
        for (i = 0; i < 8; i++)
        {
            int32_t time = time_cpw(points, i, num);
            if (time < best_time)
            {
                best_time = time;
                best_iw = i;

                for (int32_t i = 0; i < num; ++i)
                {
                    b_p[i] = points[i];
                }
            }
        }
    }
    else
    {
        for (int32_t i = k; i < num; i++)
        {
            //换序排列
            Object temp = points[k];
            points[k] = points[i];
            points[i] = temp;

            //递归下一层
            permutation_cpw(k + 1, num, points, b_p);

            //每层递归后恢复上层的顺序
            temp = points[k];
            points[k] = points[i];
            points[i] = temp;
        }
    }
}

void getBestPlan(Object *points, Object *b_p)
{
    int32_t num = 5;
    curPos = getCarPosByRound();
    best_time = 2147483640, best_iw = 0;
    permutation_cpw(0, num, points, b_p);
    b_p[5].x = wareHouse[best_iw].x, b_p[5].y = wareHouse[best_iw].y;
}