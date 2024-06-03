/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENC_A_Pin GPIO_PIN_0
#define ENC_A_GPIO_Port GPIOA
#define ENC_A_EXTI_IRQn EXTI0_IRQn
#define ENC_B_Pin GPIO_PIN_1
#define ENC_B_GPIO_Port GPIOA
#define ENC_B_EXTI_IRQn EXTI1_IRQn
#define ENC_ENT_Pin GPIO_PIN_2
#define ENC_ENT_GPIO_Port GPIOA
#define ENC_ENT_EXTI_IRQn EXTI2_IRQn
#define MODE_PB_Pin GPIO_PIN_3
#define MODE_PB_GPIO_Port GPIOA
#define MODE_PB_EXTI_IRQn EXTI3_IRQn
#define INPUT_Pin GPIO_PIN_4
#define INPUT_GPIO_Port GPIOA
#define INPUT_EXTI_IRQn EXTI4_IRQn
#define RST_DISP_Pin GPIO_PIN_0
#define RST_DISP_GPIO_Port GPIOB
#define LED_DISP_Pin GPIO_PIN_1
#define LED_DISP_GPIO_Port GPIOB
#define ESC_PB_Pin GPIO_PIN_10
#define ESC_PB_GPIO_Port GPIOB
#define ESC_PB_EXTI_IRQn EXTI15_10_IRQn
#define RST_PB_Pin GPIO_PIN_11
#define RST_PB_GPIO_Port GPIOB
#define RST_PB_EXTI_IRQn EXTI15_10_IRQn
#define ACTIVE_LOW_Pin GPIO_PIN_6
#define ACTIVE_LOW_GPIO_Port GPIOB
#define RC3_Pin GPIO_PIN_7
#define RC3_GPIO_Port GPIOB
#define RC2_Pin GPIO_PIN_8
#define RC2_GPIO_Port GPIOB
#define RC1_Pin GPIO_PIN_9
#define RC1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
