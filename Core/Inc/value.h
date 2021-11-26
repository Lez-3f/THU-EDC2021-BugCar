/*
    此文件储存了完成任务所需要的变量定义和声明
*/
#ifndef _VALUE_H_
#define _VALUE_H_

typedef struct
{
    int x, y;
}Pos;

typedef struct
{
    int x, y;
    char type;
}Object;

typedef struct
{
    int x, y;
    int vec[2];
}Car;

#endif