/**
 * @brief 本文件为小端序专用
*/
#include "gyro.h"
#include "delay.h"
#include <string.h>

volatile GyroInfoTypeDef gyroAccelerate;  // 储存加速度
volatile GyroInfoTypeDef gyroVelocity;    // 储存角速度
volatile GyroInfoTypeDef gyroAngle;       // 储存角度值
volatile float gyroTemperature;           // 储存温度值

uint8_t gyroReceive[GYRO_BUF_LENGTH];   // 实时记录收到的信息
uint8_t gyroHalfRecvBuf[GYRO_BUF_LENGTH + sizeof(gyroMessage)];  // 储存未全部被接收的消息
uint8_t gyroHalfRecvLen = 0;    // 储存未完全接收消息的长度
GyroMsgTypeDef gyroMessage;   // 解析收到的信息

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

// int8_t gyrotestCount = 0;

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
    gyroHalfRecvLen = 0;
    // 开启空闲中断
    __HAL_UART_ENABLE_IT(gyrohuart, UART_IT_IDLE);
    gyro_start_once();
}

/**
 * @brief 读取1次陀螺仪数据
 */
void gyro_start_once() {
    HAL_UART_Receive_DMA(gyrohuart, gyroReceive, GYRO_BUF_LENGTH);
}

/**
 * @brief 加速度解码
 */
inline void gyroDecodeAccelerate(void) {
    gyroAccelerate.x = (float)gyroMessage.dec.x / 32768 * 16 * GYRO_g;
    gyroAccelerate.y = (float)gyroMessage.dec.y / 32768 * 16 * GYRO_g;
    gyroAccelerate.z = (float)gyroMessage.dec.z / 32768 * 16 * GYRO_g;
}

/**
 * @brief 角速度解码
 */
inline void gyroDecodeVelocity(void) {
    gyroVelocity.x = (float)gyroMessage.dec.x / 32768 * 2000;
    gyroVelocity.y = (float)gyroMessage.dec.y / 32768 * 2000;
    gyroVelocity.z = (float)gyroMessage.dec.z / 32768 * 2000;
}

/**
 * @brief 角度值解码
 */
inline void gyroDecodeAngle(void) {
    gyroAngle.x = (float)gyroMessage.dec.x / 32768 * 180;
    gyroAngle.y = (float)gyroMessage.dec.y / 32768 * 180;
    gyroAngle.z = (float)gyroMessage.dec.z / 32768 * 180;
}

/**
 * @brief 温度值解码
 */
inline void gyroDecodeTemperature(void) {
    gyroTemperature = (float)gyroMessage.dec.temperature / 340 + 36.53;
}

/**
 * @brief 解码
 */
inline void gyroDecode(void) {
    switch (gyroMessage.dec.type) {
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
    // 停止DMA接收
    HAL_UART_DMAStop(gyrohuart);
    // 计算接收数据长度
    uint8_t data_length = GYRO_BUF_LENGTH - __HAL_DMA_GET_COUNTER(gyrohuart->hdmarx);
    // 暂存
    memcpy(gyroHalfRecvBuf + gyroHalfRecvLen, gyroReceive, data_length);
    gyroHalfRecvLen += data_length;
    // 清空缓冲区
    memset(gyroReceive, 0, GYRO_BUF_LENGTH);
    // 继续DMA接收
    gyro_start_once();

    bool succeed = false;
    uint8_t* ptr = gyroHalfRecvBuf;
    for (;ptr - gyroHalfRecvBuf <= gyroHalfRecvLen - sizeof(gyroMessage);++ptr) {
        // 寻找数据包头
        if (*ptr == 0x55) {
            memcpy(gyroMessage.buf, gyroHalfRecvBuf, sizeof(gyroMessage));
            // 校验
            uint8_t sum = 0x00;
            for (int8_t i = 0; i < sizeof(gyroMessage) - 1; ++i) {
                sum += gyroMessage.buf[i];
            }
            if (sum == gyroMessage.dec.sum) {
                // 校验成功
                succeed = true;
                ptr += sizeof(gyroMessage);
            }
        }
    }

    uint8_t* lastptr = NULL;
    for (;ptr - gyroHalfRecvBuf < gyroHalfRecvLen;++ptr) {
        // 寻找数据包头
        if (*ptr == 0x55) {
            lastptr = ptr;
            break;
        }
    }

    gyroHalfRecvLen = 0;
    if (lastptr != NULL) {
        for (uint8_t* cpyptr = gyroHalfRecvBuf;lastptr - gyroHalfRecvBuf < gyroHalfRecvLen;++cpyptr, ++lastptr) {
            *cpyptr = *lastptr;
            ++gyroHalfRecvLen;
        }
    }

    if (succeed) {
        // 校验成功
        gyroDecode();
        // ++gyrotestCount;
    } else {
        // 校验失败
        gyroError();
    }

    // ++gyrotestCount;
    // if (gyrotestCount >= 20) {
    //     HAL_GPIO_TogglePin(pinLED_GPIO_Port, pinLED_Pin);
    //     gyrotestCount -= 20;
    // }
}

/**
 * @brief 校验失败处理
 */
void gyroError(void) {
    // 稍后重启
    // HAL_UART_DMAStop(gyrohuart);
    // delay_us(500);
    // gyro_start_once();
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
