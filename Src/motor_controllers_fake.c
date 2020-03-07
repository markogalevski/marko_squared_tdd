#include "motor_controller_spy.h"


static volatile uint32_t *const encoder_contents[NUM_ENCODERS] =
    {
	(uint32_t *) &htim3.Instance, (uint32_t *) &htim4.Instance
    };

static int16_t lastError = 0;

int16_t motor_controller_spy_GetLastError()
{
  return(lastError);
}
void update_pwm_fake(TIM_HandleTypeDef *htim, int32_t error)
{
  lastError = error;
}

uint32_t motor_read_encoder_fake(encoder_t encoder)
{
  TIM_TypeDef *target =(TIM_TypeDef *) encoder_contents[encoder];
  return (target->CNT++);
}
