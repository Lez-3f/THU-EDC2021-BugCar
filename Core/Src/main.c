/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
  /* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "delay.h"
#include "pwm.h"
#include "gyro.h"
#include "ctrl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief TIM中断回调函数
 * @param htim 触发中断的TIM指针
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  static int16_t PIDCounter = 0;
  if (htim == &TIM_PID) {
    if (PIDCounter >= GyroDoubling) {
      PIDCounter -= GyroDoubling;
      TIM_PID_Callback();
    }
    ++PIDCounter;
  }
}

/**
 * @brief UART的中断(空闲中断)
 */
void HAL_USER_UART_IRQHandler(UART_HandleTypeDef* huart) {
  if (huart == &UART_GYRO) {
    // 确认是否为空闲中断
    if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
      // 清除空闲中断标志
      __HAL_UART_CLEAR_IDLEFLAG(huart);
      // 调用处理函数
      gyroMessageRecord();
    }
  }
}

/**
 * @brief 初始化
 */
void setup(void) {
  delay_init();
  gyro_init_default(&UART_GYRO);
  HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_SET);
  delay_ms(1000);
  HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
  pwm_init();
}

/**
 * @brief 主循环
 */
void loop(void) {
  // 检验小端序
  // HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, findCPUEndian());
  // delay_ms(400);
  // HAL_GPIO_TogglePin(pinLED_GPIO_Port, pinLED_Pin);
  // delay_ms(100);

  // if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) == GPIO_PIN_RESET) {
  //   HAL_GPIO_TogglePin(pinLED_GPIO_Port, pinLED_Pin);
  // } else {
  //   HAL_GPIO_WritePin(pinLED_GPIO_Port, pinLED_Pin, GPIO_PIN_RESET);
  // }

  if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
    setSpeedStraight0();
    setAngle(getRealAngle());
  } else {
    setSpeedStraight0();
    // 转弯前角度
    float angle0 = getRealAngle();
    setAngle(angle0);
    for (int i = 0;i < 4;++i) {
      setSpeedStraight(40, 30);
      while (!isDisCompleted()) {
        if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
          return;
        }
      }
      setAngle(angle0 - 90 * (i + 1));
      while (!isAngleCompleted()) {
        if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
          return;
        }
      }
    }

    // 防过热暂停
    delay_ms(1000);

    for (int i = 0;i < 4;++i) {
      setAngle(angle0 + 90 * (i + 1));
      while (!isAngleCompleted()) {
        if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
          return;
        }
      }
      setSpeedStraight(-40, 30);
      while (!isDisCompleted()) {
        if (HAL_GPIO_ReadPin(pinEnable_GPIO_Port, pinEnable_Pin) != GPIO_PIN_RESET) {
          return;
        }
      }
    }
  }
  delay_ms(20);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM4_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  setup();
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    loop();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
     /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
