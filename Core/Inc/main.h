/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PIDPeriod 5000
#define PWMPeriod 10800
#define UnitFreq 100000
#define pinRFdirN_Pin GPIO_PIN_2
#define pinRFdirN_GPIO_Port GPIOC
#define pinRFdirP_Pin GPIO_PIN_3
#define pinRFdirP_GPIO_Port GPIOC
#define pinRFspB_Pin GPIO_PIN_0
#define pinRFspB_GPIO_Port GPIOA
#define pinRFspA_Pin GPIO_PIN_1
#define pinRFspA_GPIO_Port GPIOA
#define pinTX2_Pin GPIO_PIN_2
#define pinTX2_GPIO_Port GPIOA
#define pinRX2_Pin GPIO_PIN_3
#define pinRX2_GPIO_Port GPIOA
#define pinLBspA_Pin GPIO_PIN_6
#define pinLBspA_GPIO_Port GPIOA
#define pinLBspB_Pin GPIO_PIN_7
#define pinLBspB_GPIO_Port GPIOA
#define pinLBdirP_Pin GPIO_PIN_4
#define pinLBdirP_GPIO_Port GPIOC
#define pinLBdirN_Pin GPIO_PIN_5
#define pinLBdirN_GPIO_Port GPIOC
#define pinEnable_Pin GPIO_PIN_10
#define pinEnable_GPIO_Port GPIOB
#define pinRBspB_Pin GPIO_PIN_6
#define pinRBspB_GPIO_Port GPIOC
#define pinRBspA_Pin GPIO_PIN_7
#define pinRBspA_GPIO_Port GPIOC
#define pinRBdirN_Pin GPIO_PIN_8
#define pinRBdirN_GPIO_Port GPIOC
#define pinRBdirP_Pin GPIO_PIN_9
#define pinRBdirP_GPIO_Port GPIOC
#define pinRBpwm_Pin GPIO_PIN_8
#define pinRBpwm_GPIO_Port GPIOA
#define pinLBpwm_Pin GPIO_PIN_9
#define pinLBpwm_GPIO_Port GPIOA
#define pinRFpwm_Pin GPIO_PIN_10
#define pinRFpwm_GPIO_Port GPIOA
#define pinLFpwm_Pin GPIO_PIN_11
#define pinLFpwm_GPIO_Port GPIOA
#define pinLED_Pin GPIO_PIN_2
#define pinLED_GPIO_Port GPIOD
#define pinLFspA_Pin GPIO_PIN_6
#define pinLFspA_GPIO_Port GPIOB
#define pinLFspB_Pin GPIO_PIN_7
#define pinLFspB_GPIO_Port GPIOB
#define pinLFdirP_Pin GPIO_PIN_8
#define pinLFdirP_GPIO_Port GPIOB
#define pinLFdirN_Pin GPIO_PIN_9
#define pinLFdirN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
