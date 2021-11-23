/**
 * @brief 本文件为小端序专用
*/
#include "gyro.h"
#include "delay.h"

volatile GyroInfoTypeDef gyroAccelerate;  // 储存加速度
volatile GyroInfoTypeDef gyroVelocity;    // 储存角速度
volatile GyroInfoTypeDef gyroAngle;       // 储存角度值
volatile float gyroTemperature;           // 储存温度值

GyroMsgTypeDef gyroReceive;   // 实时记录收到的信息

uint8_t gyroComInitAngle[] = { 0xFF, 0xAA, 0x52 };
uint8_t gyroComCalibrate[] = { 0xFF, 0xAA, 0x67 };
uint8_t gyroComSetBaud115200[] = { 0xFF, 0xAA, 0x63 };
uint8_t gyroComSetBaud9600[] = { 0xFF, 0xAA, 0x64 };
uint8_t gyroComSetHorizontal[] = { 0xFF, 0xAA, 0x65 };
uint8_t gyroComSetVertical[] = { 0xFF, 0xAA, 0x66 };
uint8_t gyroComSleepAwake[] = { 0xFF, 0xAA, 0x60 };

UART_HandleTypeDef* gyrohuart;
// 定义初始状态修正后的绝对角度
volatile float GYRO_REVISE_BASE = 0;

/**
 * @brief CPU大小端
 * 小端：低位低地址
 * 大端：高位低地址
 * @return true:小端序; false:大端序
 */
bool findCPUEndian(void) {
    uint16_t x = 1;
    return (*(uint8_t*)&x == 1);
}

/**
 * @brief 陀螺仪初始化
 * @param huart 串口
 */
void gyro_init(UART_HandleTypeDef* huart) {
    gyrohuart = huart;
}

/**
 * @brief 陀螺仪初始化(默认配置)
 * @param huart 串口
 */
void gyro_init_default(UART_HandleTypeDef* huart) {
    gyro_init(huart);
    gyroSetBaud(true);
    gyroSetHorizontal();
    gyroInitAngle();
    gyroCalibrate();
}

/**
 * @brief 正式启用陀螺仪
 */
void gyro_start() {
    HAL_UART_Receive_DMA(gyrohuart, gyroReceive.buf, sizeof(gyroReceive));
}

/**
 * @brief 加速度解码
 */
inline void gyroDecodeAccelerate(void) {
    gyroAccelerate.x = (float)gyroReceive.dec.x / 32768 * 16 * GYRO_g;
    gyroAccelerate.y = (float)gyroReceive.dec.y / 32768 * 16 * GYRO_g;
    gyroAccelerate.z = (float)gyroReceive.dec.z / 32768 * 16 * GYRO_g;
}

/**
 * @brief 角速度解码
 */
inline void gyroDecodeVelocity(void) {
    gyroVelocity.x = (float)gyroReceive.dec.x / 32768 * 2000;
    gyroVelocity.y = (float)gyroReceive.dec.y / 32768 * 2000;
    gyroVelocity.z = (float)gyroReceive.dec.z / 32768 * 2000;
}

/**
 * @brief 角度值解码
 */
inline void gyroDecodeAngle(void) {
    gyroAngle.x = (float)gyroReceive.dec.x / 32768 * 180;
    gyroAngle.y = (float)gyroReceive.dec.y / 32768 * 180;
    gyroAngle.z = (float)gyroReceive.dec.z / 32768 * 180;
}

/**
 * @brief 温度值解码
 */
inline void gyroDecodeTemperature(void) {
    gyroTemperature = (float)gyroReceive.dec.temperature / 340 + 36.53;
}

/**
 * @brief 解码
 */
inline void gyroDecode(void) {
    switch (gyroReceive.dec.type) {
    case 0x51:
        gyroDecodeAccelerate();
        break;
    case 0x52:
        gyroDecodeVelocity();
        break;
    case 0x53:
        gyroDecodeAngle();
        break;
    default:
        return;
    }
    gyroDecodeTemperature();
}

/**
 * @brief 实时记录信息，在每次接收完成后更新数据，重新开启中断
 */
void gyroMessageRecord(void) {
    if (gyroReceive.dec.head == 0x55) {
        uint8_t sum = 0x00;
        for (int8_t i = 0; i < sizeof(gyroReceive) - 1; ++i) {
            sum += gyroReceive.buf[i];
        }
        if (sum == gyroReceive.dec.sum) {
            // 校验成功
            gyroDecode();
        } else {
            // 校验失败后重启DMA
            HAL_UART_DMAStop(gyrohuart);
        }
    } else {
        // 校验失败后重启DMA
        HAL_UART_DMAStop(gyrohuart);
    }
    HAL_UART_Receive_DMA(gyrohuart, gyroReceive.buf, sizeof(gyroReceive));
}

/**
 * @brief 设置波特率，可选择115200或9600
 * @param Baud115200 true:115200; false:9600
 */
void gyroSetBaud(bool Baud115200) {
    if (Baud115200) {
        HAL_UART_Transmit(gyrohuart, gyroComSetBaud115200, sizeof(gyroComSetBaud115200), HAL_MAX_DELAY);
    } else {
        HAL_UART_Transmit(gyrohuart, gyroComSetBaud9600, sizeof(gyroComSetBaud9600), HAL_MAX_DELAY);
    }
}

/**
 * @brief 设置为水平放置模式
 */
void gyroSetHorizontal(void) {
    HAL_UART_Transmit(gyrohuart, gyroComSetHorizontal, sizeof(gyroComSetHorizontal), HAL_MAX_DELAY);
}

/**
 * @brief 设置为垂直放置模式
 */
void gyroSetVertical(void) {
    HAL_UART_Transmit(gyrohuart, gyroComSetVertical, sizeof(gyroComSetVertical), HAL_MAX_DELAY);
}

/**
 * @brief 初始化z轴角度为0
 */
void gyroInitAngle(void) {
    HAL_UART_Transmit(gyrohuart, gyroComInitAngle, sizeof(gyroComInitAngle), HAL_MAX_DELAY);
}

/**
 * @brief 校准加速度计
 */
void gyroCalibrate(void) {
    HAL_UART_Transmit(gyrohuart, gyroComCalibrate, sizeof(gyroComCalibrate), HAL_MAX_DELAY);
}

/**
 * @brief 设置休眠/唤醒
 */
void gyroSleepAwake() {
    HAL_UART_Transmit(gyrohuart, gyroComSleepAwake, sizeof(gyroComSleepAwake), HAL_MAX_DELAY);
}
