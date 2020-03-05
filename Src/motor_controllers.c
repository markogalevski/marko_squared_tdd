#include "motor_controllers.h"

static int16_t calculate_error(uint16_t first_distance, uint16_t second_distance, float gain);

void motor_controller_centring(TIM_HandleTypeDef *htim, uint16_t right_data, uint16_t left_data, float gain)
{
  int16_t error = calculate_error(right_data, left_data, gain);
  update_pwm_value(htim, error);
}

void update_pwm_value_impl(TIM_HandleTypeDef *htim, int32_t error)
{
  volatile uint32_t pwm_value = htim->Instance->CCR1;
  pwm_value -= error;
  htim->Instance->CCR1 = pwm_value;
}

void (*update_pwm_value)(TIM_HandleTypeDef *htim, int32_t error) = update_pwm_value_impl;

static int16_t calculate_error(uint16_t first_distance, uint16_t second_distance, float gain)
{
  if (first_distance > 130 || second_distance > 130)
    {
      return(0);
    }
  int16_t error = (int16_t)(gain*((int16_t)first_distance - second_distance));
  return(error);
}
