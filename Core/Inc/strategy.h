/*
    策略实现
*/

#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include"main.h"
#include"value.h"
#include"ctrl.h"
#include<math.h>
#include"uppercom.h"

extern Object m[2]; //metal
extern Object w[8]; //warehouse
extern Object b[3]; //Beacon

/* 计算部分 */

Pos calMetPos(int S[3], Pos P[3]);  //根据三个点的强度和三个点的位置计算出金属所在的位置

Pos calCarPosByBeacon(int d[3]);    //通过己方信标到本方位置的距离计算出本方小车所在的位置

/* 行走部分 */

void go2Point(Pos p);

/* */

#endif