#include "zigbee.h"
#include "string.h"
#include "value.h"
#include "main.h"

uint8_t zigbeeMessage[ZIGBEE_MESSAGE_LENTH];			 //经过校验后确认无误的信息
uint8_t zigbeeReceive[zigbeeReceiveLength];				 //实时记录收到的信息
uint8_t zigbeeMessageSend[4] = {0x00, 0x01, 0x02, 0x03}; //小车给上位机发送的信息
uint8_t zigbee_halfrecv_buff[ZIGBEE_MESSAGE_LENTH];		 //储存未全部被接收的消息
UART_HandleTypeDef *zigbee_huart;
uint8_t zigbee_halfrecv_len = 0; //储存未完全接收消息的长度

/***********************接口****************************/

void zigbee_Init(UART_HandleTypeDef *huart)
{
	zigbee_huart = huart;
	__HAL_UART_ENABLE_IT(zigbee_huart, UART_IT_IDLE);						//开启空闲中断
	HAL_UART_Receive_DMA(zigbee_huart, zigbeeReceive, zigbeeReceiveLength); //开启DMA中断接收
}

void zigbeeMessageRecord(uint8_t rx_length)
{
	if (rx_length == ZIGBEE_MESSAGE_LENTH && zigbeeReceive[ZIGBEE_MESSAGE_LENTH - 2] == 0x0D && zigbeeReceive[ZIGBEE_MESSAGE_LENTH - 1] == 0x0A)
	{
		memcpy(zigbeeMessage, zigbeeReceive, ZIGBEE_MESSAGE_LENTH);
	}
	else if (rx_length < ZIGBEE_MESSAGE_LENTH) //用于处理单条消息分两次发送的情况
	{
		if (!zigbee_halfrecv_len)
		{
			zigbee_halfrecv_len = rx_length;
			memcpy(zigbee_halfrecv_buff, zigbeeReceive, rx_length);
			return;
		}
		else if (zigbee_halfrecv_len && rx_length + zigbee_halfrecv_len == ZIGBEE_MESSAGE_LENTH)
		{
			if ((zigbeeReceive[rx_length - 2] == 0x0D && zigbeeReceive[rx_length - 1] == 0x0A) || (rx_length == 1 && zigbeeReceive[0] == 0x0A && zigbee_halfrecv_buff[ZIGBEE_MESSAGE_LENTH - 2] == 0x0D))
			{
				memcpy(zigbeeMessage, zigbee_halfrecv_buff, zigbee_halfrecv_len);
				memcpy(zigbeeMessage + zigbee_halfrecv_len, zigbeeReceive, rx_length);
			}
			else
				return;

			zigbee_halfrecv_len = 0;
		}
		else
		{
			zigbee_halfrecv_len = 0;
			return;
		}
	}
	else
		return;

	// HAL_GPIO_TogglePin(pinLED_GPIO_Port, pinLED_Pin);
	return;
}

void zigbeeSend(int MineType)
{
	HAL_UART_Transmit(zigbee_huart, zigbeeMessageSend + MineType, 1, HAL_MAX_DELAY);
}

int32_t getGameTime(void)
{
	return (int32_t)(((uint16_t)zigbeeMessage[0] << 8) + (uint16_t)zigbeeMessage[1]);
}

int32_t getGameState(void)
{
	return (int32_t)((zigbeeMessage[2] & 0xC0) >> 6);
}

int32_t getCarTask(void)
{
	// return 0;
	return (int32_t)((zigbeeMessage[2] & 0x20) >> 5);
}

int32_t getIsMineIntensityValid(int MineNo)
{
	if (MineNo == 0)
	{
		return (int32_t)((zigbeeMessage[2] & 0x10) >> 4);
	}
	else if (MineNo == 1)
	{
		return (int32_t)((zigbeeMessage[2] & 0x08) >> 3);
	}
	else
	{
		return (int32_t)INVALID_ARG;
	}
}

int32_t getCarPosX()
{
	return (int32_t)(zigbeeMessage[3]);
}

int32_t getCarPosY()
{
	return (int32_t)(uint16_t)(zigbeeMessage[4]);
}

int32_t getMineArrayType(int MineNo)
{
	if (MineNo == 0)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[5] & 0xC0) >> 6);
	}
	else if (MineNo == 1)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[5] & 0x30) >> 4);
	}
	else
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
}

int32_t getCarMineSumNum(void)
{
	return (int32_t)(uint16_t)(zigbeeMessage[5] & 0x0F);
}

int32_t getCarMineANum(void)
{
	return (int32_t)(uint16_t)((zigbeeMessage[6] & 0xF0) >> 4);
}

int32_t getCarMineBNum(void)
{
	return (int32_t)(uint16_t)(zigbeeMessage[6] & 0x0F);
}

int32_t getCarMineCNum(void)
{
	return (int32_t)(uint16_t)((zigbeeMessage[7] & 0xF0) >> 4);
}

int32_t getCarMineDNum(void)
{
	return (int32_t)(uint16_t)(zigbeeMessage[7] & 0x0F);
}

int32_t getMyBeaconMineType(int MineType)
{
	if (MineType == 0)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[8] & 0xC0) >> 6);
	}
	else if (MineType == 1)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[8] & 0x30) >> 4);
	}
	else if (MineType == 2)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[8] & 0x0C) >> 2);
	}
	else
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
}

int32_t getMineIntensity(int MineNo)
{
	if (MineNo == 0 || MineNo == 1)
	{
		// return (int32_t)(uint32_t)0xffff;
		return (int32_t)((((uint32_t)zigbeeMessage[9 + 4 * MineNo]) << 24) + (((uint32_t)zigbeeMessage[10 + 4 * MineNo]) << 16) + (((uint32_t)zigbeeMessage[11 + 4 * MineNo]) << 8) + ((uint32_t)zigbeeMessage[12 + 4 * MineNo]));
	}
	else
	{
		return (int32_t)(uint32_t)INVALID_ARG;
	}
}

int32_t getDistanceOfMyBeacon(int BeaconNo)
{
	if (BeaconNo != 0 && BeaconNo != 1 && BeaconNo != 2)
		return (int32_t)(uint16_t)INVALID_ARG;
	else
		return (int32_t)(uint16_t)(((uint16_t)zigbeeMessage[17 + 2 * BeaconNo] << 8) + (uint16_t)zigbeeMessage[18 + 2 * BeaconNo]);
}

int32_t getDistanceOfRivalBeacon(int BeaconNo)
{
	if (BeaconNo != 0 && BeaconNo != 1 && BeaconNo != 2)
		return (int32_t)(uint16_t)INVALID_ARG;
	else
		return (int32_t)(uint16_t)(((uint16_t)zigbeeMessage[23 + 2 * BeaconNo] << 8) + (uint16_t)zigbeeMessage[24 + 2 * BeaconNo]);
}

int32_t getParkDotMineType(int ParkDotNo)
{
	if (ParkDotNo < 0 || ParkDotNo > 7)
		return (int32_t)(uint16_t)INVALID_ARG;
	else
		switch (ParkDotNo)
		{
		case 0:
			return (int32_t)(uint16_t)((zigbeeMessage[29] & 0xC0) >> 6);
			break;
		case 1:
			return (int32_t)(uint16_t)((zigbeeMessage[29] & 0x30) >> 4);
			break;
		case 2:
			return (int32_t)(uint16_t)((zigbeeMessage[29] & 0x0C) >> 2);
			break;
		case 3:
			return (int32_t)(uint16_t)((zigbeeMessage[29] & 0x03));
			break;
		case 4:
			return (int32_t)(uint16_t)((zigbeeMessage[30] & 0xC0) >> 6);
			break;
		case 5:
			return (int32_t)(uint16_t)((zigbeeMessage[30] & 0x30) >> 4);
			break;
		case 6:
			return (int32_t)(uint16_t)((zigbeeMessage[30] & 0x0C) >> 2);
			break;
		case 7:
			return (int32_t)(uint16_t)((zigbeeMessage[30] & 0x03));
			break;
		default:
			return (int32_t)(uint16_t)((zigbeeMessage[30] & 0x03));
		}
}

int32_t getCarZone(void)
{
	return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x80) >> 7);
}

int32_t getIsCarPosValid(void)
{
	return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x40) >> 6);
}

int32_t getIsDistanceOfMyBeaconValid(int BeaconNo)
{
	if (BeaconNo == 0)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x20) >> 5);
	}
	else if (BeaconNo == 1)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x10) >> 4);
	}
	else if (BeaconNo == 2)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x08) >> 3);
	}
	else
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
}

int32_t getIsDistanceOfRivalBeaconValid(int BeaconNo)
{
	if (BeaconNo == 0)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x04) >> 2);
	}
	else if (BeaconNo == 1)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x02) >> 1);
	}
	else if (BeaconNo == 2)
	{
		return (int32_t)(uint16_t)((zigbeeMessage[31] & 0x01));
	}
	else
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
}

int32_t getCarScore(void)
{
	return (int32_t)(int16_t)(((int16_t)zigbeeMessage[32] << 8) + (int16_t)zigbeeMessage[33]);
}

int32_t getMyBeaconPosX(int BeaconNo)
{
	if (BeaconNo != 0 && BeaconNo != 1 && BeaconNo != 2)
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
	else
	{
		return (int32_t)(uint16_t)(zigbeeMessage[34 + 2 * BeaconNo]);
	}
}

int32_t getMyBeaconPosY(int BeaconNo)
{
	if (BeaconNo != 0 && BeaconNo != 1 && BeaconNo != 2)
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
	else
	{
		return (int32_t)(uint16_t)(zigbeeMessage[35 + 2 * BeaconNo]);
	}
}

int32_t getRivalBeaconPosX(int BeaconNo)
{
	if (BeaconNo != 0 && BeaconNo != 1 && BeaconNo != 2)
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
	else
	{
		return (int32_t)(uint16_t)(zigbeeMessage[40 + 2 * BeaconNo]);
	}
}

int32_t getRivalBeaconPosY(int BeaconNo)
{
	if (BeaconNo != 0 && BeaconNo != 1 && BeaconNo != 2)
	{
		return (int32_t)(uint16_t)INVALID_ARG;
	}
	else
	{
		return (int32_t)(uint16_t)(zigbeeMessage[41 + 2 * BeaconNo]);
	}
}

Pos getCarPos(void)
{
	/* test*/
	Pos a = {(int32_t)getCarPosX(), (int32_t)getCarPosY()};

	return a;
}

Pos getRivalBeaconPos(int BeaconNo)
{
	Pos a = {(int32_t)getRivalBeaconPosX(BeaconNo), (int32_t)getRivalBeaconPosY(BeaconNo)};
	return a;
}

Pos getMyBeaconPos(int BeaconNo)
{
	Pos a = {(int32_t)getMyBeaconPosX(BeaconNo), (int32_t)getMyBeaconPosY(BeaconNo)};
	return a;
}

/***************************************************/
