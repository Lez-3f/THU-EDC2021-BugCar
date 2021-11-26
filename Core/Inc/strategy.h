/*
    策略实现
*/

#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include"value.h"
#include<stdbool.h>
#define SPEED 50

/* 通信定义 */

#define ROUND_1 0
#define ROUND_2 1

extern Object wareHouse[8]; //warehouse

/* 计算部分 */

Pos calMetPos(int S[3], Pos P[3]);  //根据三个点的强度和三个点的位置计算出金属所在的位置

Pos calCarPosByBeacon(int d[3], Pos b[3]);    //通过己方信标到本方位置的距离计算出本方小车所在的位置

/* 行走部分 */

bool isEnable();

void go2Point(Pos p);

/* */

#endif