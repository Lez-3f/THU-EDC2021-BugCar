#ifndef _ROUND_2_H_
#define _ROUND_2_H_

#include"value.h"

extern Pos carPos; //小车当前位置，由到信标距离算的

void loop2();

void initBeacon2(); //初始化信标位置

Pos getBestPoint();

void send2WareHouseLAZY();  //把金属送到仓库

#endif