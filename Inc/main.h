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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "robot.h"
#include "pinout.h"
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
#define RIGHT_MOTOR_PWM_PIN_Pin GPIO_PIN_0
#define RIGHT_MOTOR_PWM_PIN_GPIO_Port GPIOA
#define FLOOR_SENSOR_INT_PIN_Pin GPIO_PIN_1
#define FLOOR_SENSOR_INT_PIN_GPIO_Port GPIOA
#define FLOOR_SENSOR_INT_PIN_EXTI_IRQn EXTI1_IRQn
#define LEFT_SENSOR_INT_PIN_Pin GPIO_PIN_2
#define LEFT_SENSOR_INT_PIN_GPIO_Port GPIOA
#define LEFT_SENSOR_INT_PIN_EXTI_IRQn EXTI2_IRQn
#define RIGHT_SENSOR_INT_PIN_Pin GPIO_PIN_3
#define RIGHT_SENSOR_INT_PIN_GPIO_Port GPIOA
#define RIGHT_SENSOR_INT_PIN_EXTI_IRQn EXTI3_IRQn
#define LEFT_MOTOR_PWM_PIN_Pin GPIO_PIN_5
#define LEFT_MOTOR_PWM_PIN_GPIO_Port GPIOA
#define ENCODER_L_B_Pin GPIO_PIN_7
#define ENCODER_L_B_GPIO_Port GPIOA
#define BUTTON_INT_PIN_Pin GPIO_PIN_5
#define BUTTON_INT_PIN_GPIO_Port GPIOC
#define BUTTON_INT_PIN_EXTI_IRQn EXTI9_5_IRQn
#define RIGHT_MOTOR_REVERSE_Pin GPIO_PIN_14
#define RIGHT_MOTOR_REVERSE_GPIO_Port GPIOB
#define LEFT_MOTOR_REVERSE_Pin GPIO_PIN_15
#define LEFT_MOTOR_REVERSE_GPIO_Port GPIOB
#define FRONT_SENSOR_INT_PIN_Pin GPIO_PIN_13
#define FRONT_SENSOR_INT_PIN_GPIO_Port GPIOA
#define FRONT_SENSOR_INT_PIN_EXTI_IRQn EXTI15_10_IRQn
#define YELLOW_LED_Pin GPIO_PIN_12
#define YELLOW_LED_GPIO_Port GPIOC
#define ENCODER_L_A_Pin GPIO_PIN_4
#define ENCODER_L_A_GPIO_Port GPIOB
#define ENCODER_R_A_Pin GPIO_PIN_6
#define ENCODER_R_A_GPIO_Port GPIOB
#define ENCODER_R_B_Pin GPIO_PIN_7
#define ENCODER_R_B_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
