/**
 * @brief 本文件为小端序专用
*/
#ifndef _GYRO_H_
#define _GYRO_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

// 定义重力加速度值
#define GYRO_g 9.8
// 定义初始状态修正后的绝对角度
extern volatile float GYRO_REVISE_BASE;

typedef struct {
    float x;
    float y;
    float z;
} GyroInfoTypeDef;

#pragma pack(1)
typedef union {
    uint8_t buf[11];
    struct {
        uint8_t head;
        uint8_t type;
        int16_t x;
        int16_t y;
        int16_t z;
        uint16_t temperature;
        uint8_t sum;
    } dec;
} GyroMsgTypeDef;
#pragma pack()

extern volatile GyroInfoTypeDef gyroAccelerate;  // 储存加速度
extern volatile GyroInfoTypeDef gyroVelocity;    // 储存角速度
extern volatile GyroInfoTypeDef gyroAngle;       // 储存角度值
extern volatile float gyroTemperature;           // 储存温度值
extern GyroMsgTypeDef gyroReceive;

bool findCPUEndian(void);   // CPU大小端
void gyro_init(UART_HandleTypeDef* huart);  // 初始化
void gyro_init_default(UART_HandleTypeDef* huart);  // 初始化(默认)
void gyro_start();  // 正式启用
void gyroMessageRecord(void);   // 实时记录信息，在每次接收完成后更新数据，重新开启中断

void gyroSetBaud(bool Baud115200); // 设置波特率，可选择115200或9600
void gyroSetHorizontal(void);   // 设置为水平放置模式
void gyroSetVertical(void); // 设置为垂直放置模式
void gyroInitAngle(void);   // 初始化z轴角度为0
void gyroCalibrate(void);   // 校准加速度计
void gyroSleepAwake(void);    // 设置休眠/唤醒

#endif // !_GYRO_H_
