/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f0xx_hal.h"

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
#define FAN_SW_Pin GPIO_PIN_0
#define FAN_SW_GPIO_Port GPIOF
#define LED_CHG_Pin GPIO_PIN_1
#define LED_CHG_GPIO_Port GPIOF
#define CHG_STAT_Pin GPIO_PIN_0
#define CHG_STAT_GPIO_Port GPIOA
#define AMP_SD_Pin GPIO_PIN_5
#define AMP_SD_GPIO_Port GPIOA
#define BST_EN_Pin GPIO_PIN_6
#define BST_EN_GPIO_Port GPIOA
#define VBAT_DET_Pin GPIO_PIN_7
#define VBAT_DET_GPIO_Port GPIOA
#define AMP_SW_Pin GPIO_PIN_0
#define AMP_SW_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_8
#define LED_G_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_9
#define LED_R_GPIO_Port GPIOA
#define LED_B_Pin GPIO_PIN_10
#define LED_B_GPIO_Port GPIOA
#define VBUS_DET_Pin GPIO_PIN_7
#define VBUS_DET_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
