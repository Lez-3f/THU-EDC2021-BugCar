/*
    策略实现
*/

#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include"value.h"
#include<stdbool.h>
#define SPEED 50
#define SPEED_SLOW 20

/* 通信定义 */

#define ROUND_1 0
#define ROUND_2 1

extern Object wareHouse[8]; //warehouse
extern Object metal[2];
extern Object beacon[3];

/* 计算部分 */

Pos calMetPos(int32_t S[3], Pos P[3]);  //根据三个点的强度和三个点的位置计算出金属所在的位置

Pos getCarPosByBeacon();
Pos calCarPosByBeacon(int32_t d[3], Pos b[3]);    //通过己方信标到本方位置的距离计算出本方小车所在的位置

/* 行走部分 */

#define TRI_D_M 20  //测量金属所用的三角形边长
#define X_B 132
#define Y_B 129
#define TRI_D_B 40  //测量小车位置所用的信标三角形边长

bool isEnable();

void go2Point(Pos p);

void measureMetalPos0();

void initWareHouse();

bool move(float angle, float distance, float speed);

bool turnAngle(float angle);    //转至设定角度角度
bool goStraight(float speed, float destance);   //直走

void measureMetalPos(); //第二回合才能用

Pos getCarPosByRound(); //根据情况获得小车当前位置

/* */

#endif