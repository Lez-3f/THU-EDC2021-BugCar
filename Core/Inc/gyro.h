/**
 * @brief 本文件为小端序专用
*/
#ifndef _GYRO_H_
#define _GYRO_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

#define JY62_MESSAGE_LENGTH 11
#define g 9.8   //定义重力加速度值

typedef struct {
    float x;
    float y;
    float z;
} GyroInfoTypeDef;

extern GyroInfoTypeDef GyroAccelerate;  // 储存加速度
extern GyroInfoTypeDef GyroVelocity;    // 储存角速度
extern GyroInfoTypeDef GyroAngle;       // 储存角度值
extern float GyroTemperature;           // 储存温度值

bool findCPUEndian(void);
void jy62_Init(UART_HandleTypeDef* huart);    //初始化
void jy62MessageRecord(void);							//实时记录信息，在每次接收完成后更新数据，重新开启中断

/**
 * @brief Set the Baud object
 * 
 * @param Baud 
 */
void SetBaud(int Baud);     //设置波特率，可选择115200或9600
void SetHorizontal(void);  //设置为水平放置模式
void SetVertical(void);  //设置为垂直放置模式
void InitAngle(void);    //初始化z轴角度为0
void Calibrate(void);    //校准加速度计
void SleepOrAwake(void);   //设置休眠/唤醒

float GetVeloX(void);       //获得角速度
float GetVeloY(void);
float GetVeloZ(void);

float GetAccX(void);        //获得加速度
float GetAccY(void);
float GetAccZ(void);

float GetRoll(void);     //绕x轴旋转角度（横滚角）
float GetPitch(void);    //绕y轴旋转角度（俯仰角）
float GetYaw(void);     //绕z轴旋转角度（偏航角）
float GetTemperature(void);


/****************************************************/
void Decode();

#endif // !_GYRO_H_
