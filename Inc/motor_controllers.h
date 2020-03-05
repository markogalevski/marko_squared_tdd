#ifndef _MOTOR_CONTROLLERS_H
#define _MOTOR_CONTROLLERS_H
#include <stdint.h>
#include "stm32f4xx_hal.h"

#define CENTRING_P_GAIN		(float) 0.5
#define CURVE_P_GAIN		(float) 0.5

void motor_controller_centring(TIM_HandleTypeDef *htim, uint16_t right_data, uint16_t left_data, float gain);
extern void (*update_pwm_value)(TIM_HandleTypeDef *htim, int32_t error);
#endif
