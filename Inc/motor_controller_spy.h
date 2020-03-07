#ifndef _MOTOR_CONTROLLER_SPY_H
#define _MOTOR_CONTROLLER_SPY_H

#include "motor_controllers.h"

int16_t motor_controller_spy_GetLastError();
void update_pwm_fake(TIM_HandleTypeDef *htim, int32_t error);
uint32_t motor_read_encoder_fake(encoder_t encoder);
#endif
