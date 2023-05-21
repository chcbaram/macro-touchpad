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
#include "stm32g0xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_TP_RESET_Pin GPIO_PIN_5
#define LCD_TP_RESET_GPIO_Port GPIOA
#define I2S_SD_MODE_Pin GPIO_PIN_0
#define I2S_SD_MODE_GPIO_Port GPIOB
#define V3V3_OFF_Pin GPIO_PIN_1
#define V3V3_OFF_GPIO_Port GPIOB
#define TOUCH_OUT_Pin GPIO_PIN_8
#define TOUCH_OUT_GPIO_Port GPIOA
#define POWER_BTN_Pin GPIO_PIN_6
#define POWER_BTN_GPIO_Port GPIOC
#define CHARGE_FLAG_Pin GPIO_PIN_15
#define CHARGE_FLAG_GPIO_Port GPIOA
#define PB5_LED_Pin GPIO_PIN_5
#define PB5_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
