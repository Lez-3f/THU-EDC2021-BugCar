/*
    此文件储存了完成任务所需要的变量定义和声明
*/
#ifndef _VALUE_H_
#define _VALUE_H_
#include <stdint.h>

typedef struct
{
    int32_t x, y;
}Pos;

typedef struct
{
    int32_t x, y;
    char type;
}Object;

typedef struct
{
    int32_t x, y;
    int32_t vec[2];
}Car;

#endif