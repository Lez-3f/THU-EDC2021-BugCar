/**
 * @brief 本文件为小端序专用
*/
#include "gyro.h"

/**
 * @brief CPU大小端
 * 小端：低位低地址
 * 大端：高位低地址
 * @return 0,大端序;  1,小端序.
 */
bool findCPUEndian(void) {
    uint16_t x = 1;
    return (*(uint8_t*)&x == 1);
}