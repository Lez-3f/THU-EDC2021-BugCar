/********************************
zigbee.h
接受上位机的数据
接收说明（以USART1为例）
    接收信息前需要开启USART1_Rx的DMA中断以及USART1全局中断
    在程序开始的时候使用zigbee_Init(&huart1)进行初始化;
    在回调函数中使用zigbeeMessageRecord(void)记录数据，并重新开启中断
    接收到的数据存储在zigbeeMessage[]中
    通过以下接口获取数据

**********************************/

#ifndef ZIGBEE_H
#define ZIGBEE_H
#include "stm32f1xx_hal.h"
#include "value.h"
#define INVALID_ARG 0xffffffff          //错误位
#define ZIGBEE_MESSAGE_LENTH 48         //单条消息长度
#define zigbeeReceiveLength  255    //接收缓冲区长度    

/**************接口*************************/

/*
函数输入说明
    输入的mineNo范围为0-1
    BeaconNo范围为0-2，按照放置顺序编号
    ParkDotNo范围为0-7
    MineType范围为0-3,依次对应A，B，C，D四种金矿
*/


void zigbee_Init(UART_HandleTypeDef* huart);    //初始化
void zigbeeMessageRecord(uint8_t length);							//实时记录信息，在每次中断处理完成后重新开启中断
/**
 * @brief 小车放置信标发送信息
 * @param Minetype 信标仓库种类 0-3
 */
void zigbeeSend(int MineType);   
/**
 * @brief 获取比赛时间，单位为0.1s,范围为0-120s
 */
uint16_t getGameTime(void);
/**
 * @brief 获取比赛状态，0为未开始，1为进行中，2为暂停，3为已结束
 */
uint8_t getGameState(void);			
/**
 * @brief 获取车辆任务状态，0为上半场，1为下半场
 */
uint16_t getCarTask(void);
/**
 * @brief 获取金矿强度是否有效 有效为1，无效为0
 * @param MineNo 场上金属序号：0,1
 */
uint16_t getIsMineIntensityValid(int MineNo);
/**
 * @brief 获取场上2个金矿的种类
 * @param MineNo 场上金属序号：0,1
 */
uint16_t getMineArrayType(int MineNo);  
/**
 * @brief 获取8个停车点能够存储的金矿种类
 * @param ParkDotNo 八个仓库序号：0-7
 */              
uint16_t getParkDotMineType(int ParkDotNo);       
/**
 * @brief 获取3个己方信标充当仓库可存储的金矿类型
 * @param BeaconNo 信标序号0-2
 */         
uint16_t getMyBeaconMineType(int BeaconNo);      
/**
 * @brief 获取车辆目前载有金矿的总数目
 */           
uint16_t getCarMineSumNum(void);   
/**
 * @brief  获取车辆目前载有金矿A的数目
 */                             
uint16_t getCarMineANum(void);             
/**
 * @brief 获取车辆目前载有金矿B的数目
 */                  
uint16_t getCarMineBNum(void);   
/**
 * @brief  获取车辆目前载有金矿C的数目
 */                             
uint16_t getCarMineCNum(void);    
/**
 * @brief 获取车辆目前载有金矿D的数目
 */                           
uint16_t getCarMineDNum(void);    
/**
 * @brief  获取小车x坐标
 */                            
uint16_t getCarPosX(void);	
/**
 * @brief 获取小车y坐标
 */	        
uint16_t getCarPosY(void);		
/**
 * @brief 获取小车中心处的两金矿强度
 * @param MineNo 场上金属序号：0,1
 */	                           
uint32_t getMineIntensity(int MineNo);    
    /**
 * @brief 获取小车到己方3个信标的距离
 * @param BeaconNo 信标序号 0-2
 */	            
uint16_t getDistanceOfMyBeacon(int BeaconNo); 
    /**
 * @brief 获取己方3个信标的x坐标
 * @param BeaconNo 信标序号 0-2
 */	      
uint16_t getMyBeaconPosX(int BeaconNo);  
    /**
 * @brief 获取己方3个信标的y坐标
 * @param BeaconNo 信标序号 0-2
 */	          
uint16_t getMyBeaconPosY(int BeaconNo);   
    /**
 * @brief 获取小车到对方3个信标的距离
 * @param BeaconNo 信标序号 0-2
 */	           
uint16_t getDistanceOfRivalBeacon(int BeaconNo);  
    /**
 * @brief 获取对方3个信标的x坐标
 *  @param BeaconNo 信标序号 0-2
 */	   
uint16_t getRivalBeaconPosX(int BeaconNo);        
    /**
 * @brief 获取对方3个信标的y坐标
 *  @param BeaconNo 信标序号 0-2
 */	   
uint16_t getRivalBeaconPosY(int BeaconNo);        
    /**
 * @brief 获取车辆所在区域信息，0为周边道路，1为中央矿区
 */	   
uint16_t getCarZone(void);       
    /**
 * @brief 获取车辆位置信息是否有效
 */	                                 
uint16_t getIsCarPosValid(void);                    
    /**
 * @brief 获取小车到己方3个信标距离（信标坐标）是否有效
 *  @param BeaconNo 信标序号 0-2
 */	           
uint16_t getIsDistanceOfMyBeaconValid(int BeaconNo); 
    /**
 * @brief 获取小车到对方3个信标距离（信标坐标）是否有效
 *  @param BeaconNo 信标序号 0-2
 */	       
uint16_t getIsDistanceOfRivalBeaconValid(int BeaconNo);  
      /**
 * @brief 获取车辆得分
 */	 
int16_t getCarScore(void);            
    /**
 * @brief 获取小车位置
 */	                        
Pos getCarPos(void);   
    /**
 * @brief 获获取对方信标位置
 *  @param BeaconNo 信标序号 0-2
 */	                                           
Pos getRivalBeaconPos(int BeaconNo);  
    /**
 * @brief 我方信标位置
 *  @param BeaconNo 信标序号 0-2
 */
Pos getMyBeaconPos(int BeaconNo);

#endif 