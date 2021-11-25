#ifndef _UPPERCOM_H_
#define _UPPERCOM_H_

#include"main.h"
#include "value.h"

//通用
uint8_t GameState;                 //比赛开始标志
uint32_t MineIntensity[2];         //场上金属强度

//回合1
Pos CarPos;                //小车位置

//回合2
Pos MyBeaconPos[3];          //我方信标位置
Pos RivalBeaconPos[3];       //对方信标位置 
uint16_t CarMineSumNum;              //小车现有金属
uint16_t CarMineANum;                //小车现有A金属
uint16_t CarMineBNum;
uint16_t CarMineCNum;
uint16_t CarMineDNum;
uint16_t MineArrayType[2];            //场上现有金属种类
uint16_t ParkDotMineType[8];          //仓库类型
uint16_t MyBeaconMineType[3];          //我方信标仓库类型
uint16_t DistanceOfMyBeacon[3];         //小车距我方信标距离
uint16_t DistanceOfRivalBeacon[3];      //小车距对方信标距离


//存储
Pos MemoryCarPos[3];
uint32_t MemoryMineIntensity1[3];
uint32_t MemoryMineIntensity2[3];

#endif