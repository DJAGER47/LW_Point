/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY3_Pin GPIO_PIN_2
#define KEY3_GPIO_Port GPIOC
#define G_Pin GPIO_PIN_3
#define G_GPIO_Port GPIOC
#define TIM2_CH1_Res_Pin GPIO_PIN_0
#define TIM2_CH1_Res_GPIO_Port GPIOA
#define RCK_Pin GPIO_PIN_1
#define RCK_GPIO_Port GPIOA
#define SRCK_Pin GPIO_PIN_2
#define SRCK_GPIO_Port GPIOA
#define SERIN_Pin GPIO_PIN_3
#define SERIN_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_4
#define FLASH_CS_GPIO_Port GPIOA
#define FLASH_SCK_Pin GPIO_PIN_5
#define FLASH_SCK_GPIO_Port GPIOA
#define FLASH_MISO_Pin GPIO_PIN_6
#define FLASH_MISO_GPIO_Port GPIOA
#define FLASH_MOSI_Pin GPIO_PIN_7
#define FLASH_MOSI_GPIO_Port GPIOA
#define LED_CS_Pin GPIO_PIN_2
#define LED_CS_GPIO_Port GPIOB
#define LED_RES_Pin GPIO_PIN_10
#define LED_RES_GPIO_Port GPIOB
#define LED_DC_Pin GPIO_PIN_11
#define LED_DC_GPIO_Port GPIOB
#define MUTE_Pin GPIO_PIN_14
#define MUTE_GPIO_Port GPIOB
#define TIM3_CH1_Transmit_Pin GPIO_PIN_6
#define TIM3_CH1_Transmit_GPIO_Port GPIOC
#define INT_TIM1_CH1_Pin GPIO_PIN_8
#define INT_TIM1_CH1_GPIO_Port GPIOA
#define INT_TIM1_CH2_Pin GPIO_PIN_9
#define INT_TIM1_CH2_GPIO_Port GPIOA
#define INT_TIM1_CH3_Pin GPIO_PIN_10
#define INT_TIM1_CH3_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_11
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_12
#define KEY2_GPIO_Port GPIOA
#define ESP_EN_Pin GPIO_PIN_15
#define ESP_EN_GPIO_Port GPIOA
#define ESP_TX_Pin GPIO_PIN_10
#define ESP_TX_GPIO_Port GPIOC
#define ESP_RX_Pin GPIO_PIN_11
#define ESP_RX_GPIO_Port GPIOC
#define LED_SCK_Pin GPIO_PIN_3
#define LED_SCK_GPIO_Port GPIOB
#define LED_MISO_Pin GPIO_PIN_4
#define LED_MISO_GPIO_Port GPIOB
#define LED_MOSI_Pin GPIO_PIN_5
#define LED_MOSI_GPIO_Port GPIOB
#define EXT_TIM4_CH1_Pin GPIO_PIN_6
#define EXT_TIM4_CH1_GPIO_Port GPIOB
#define EXT_TIM4_CH2_Pin GPIO_PIN_7
#define EXT_TIM4_CH2_GPIO_Port GPIOB
#define EXT_TIM4_CH3_Pin GPIO_PIN_8
#define EXT_TIM4_CH3_GPIO_Port GPIOB
#define KEY4_Pin GPIO_PIN_9
#define KEY4_GPIO_Port GPIOB
void   MX_TIM3_Init(void);
void   MX_TIM5_Init(void);
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
