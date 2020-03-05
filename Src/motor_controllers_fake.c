#include "motor_controller_spy.h"

static int16_t lastError = 0;

int16_t motor_controller_spy_GetLastError()
{
  return(lastError);
}
void update_pwm_fake(TIM_HandleTypeDef *htim, int32_t error)
{
  lastError = error;
}
