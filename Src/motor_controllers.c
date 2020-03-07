#include "motor_controllers.h"


static volatile uint32_t *const encoder_contents[NUM_ENCODERS] =
    {
	(uint32_t *) &htim3.Instance, (uint32_t *) &htim4.Instance
    };

static TIM_HandleTypeDef *motor_handles[NUM_MOTORS] =
    {
	(TIM_HandleTypeDef *)  &htim2, (TIM_HandleTypeDef *) &htim5
    };

static uint32_t referenceLeftEncoder = 0;
static uint32_t referenceRightEncoder = 0;

static uint32_t *reference_encoders[NUM_ENCODERS] =
    {
	&referenceLeftEncoder, &referenceRightEncoder
    };


static void motor_enable(motor_t motor);
static void motor_disable(motor_t motor);
static void motor_set_forward(motor_t motor);
static void motor_set_reverse(motor_t motor);
static void motor_centering(TIM_HandleTypeDef *controlled_motor, uint16_t right_data, uint16_t left_data, float gain);
static int16_t calculate_error(uint16_t first_distance, uint16_t second_distance, float gain);

void motor_controller_forward(uint16_t right_data, uint16_t left_data, float gain)
{
  TIM_HandleTypeDef *controlled_motor= motor_handles[MOTOR_L];
  motor_centering(controlled_motor, right_data, left_data, gain);
  motor_set_forward(MOTOR_R);
  motor_set_forward(MOTOR_L);
  motor_enable_all();
}

void motor_controller_in_place(in_place_t direction)
{
  encoder_t outer_encoder;
  motor_t outer_motor, inner_motor;
  uint32_t target_reading = ENCODER_90_TURN;
  if (direction == LEFT_90_DEG)
    {
      outer_encoder = ENCODER_R;
      outer_motor = MOTOR_R;
      inner_motor = MOTOR_L;
    }
  else if (direction == RIGHT_90_DEG)
    {
      outer_encoder = ENCODER_L;
      outer_motor = MOTOR_L;
      inner_motor = MOTOR_R;
    }
  else if (direction == LEFT_180_DEG)
    {
      outer_encoder = ENCODER_R;
      outer_motor = MOTOR_R;
      inner_motor = MOTOR_L;
      target_reading = ENCODER_180_TURN;
    }
  motor_disable_all();
  motor_set_forward(outer_motor);
  motor_set_reverse(inner_motor);
  motor_enable_all();
  uint32_t start_encoder = motor_read_encoder(outer_encoder);
  uint32_t end_encoder = start_encoder;
  while(end_encoder - start_encoder < target_reading)
    {
      end_encoder = motor_read_encoder(outer_encoder);
    }
  motor_disable_all();
}

void motor_controller_coast(float coast_cm)
{

  float coast_ticks = CM_TO_TICKS(coast_cm);
  uint32_t current_encoder = motor_read_encoder(ENCODER_R);
  uint32_t end_encoder = current_encoder;

  while (end_encoder - current_encoder < coast_ticks)
    {
      end_encoder = motor_read_encoder(ENCODER_R);
    }
  motor_disable_all();
}

uint32_t motor_read_encoder_impl(encoder_t encoder)
{
  TIM_TypeDef *target =(TIM_TypeDef *) encoder_contents[encoder];
  return (target->CNT++);
}
uint32_t (*motor_read_encoder)(encoder_t encoder) = motor_read_encoder_impl;

uint32_t motor_read_reference_encoder(encoder_t encoder)
{
  return(reference_encoders[encoder]);
}

static void motor_centering(TIM_HandleTypeDef *controlled_motor, uint16_t right_data, uint16_t left_data, float gain)
{
  int16_t error = calculate_error(right_data, left_data, gain);
  update_pwm_value(controlled_motor, error);
}



static void update_pwm_value_impl(TIM_HandleTypeDef *htim, int32_t error)
{
  volatile uint32_t pwm_value = htim->Instance->CCR1;
  pwm_value -= error;
  htim->Instance->CCR1 = pwm_value;
}
void (*update_pwm_value)(TIM_HandleTypeDef *htim, int32_t error) = update_pwm_value_impl;


void motor_reset_reference_encoder_values(void)
{
	referenceLeftEncoder = motor_read_encoder(ENCODER_L);
	referenceRightEncoder = motor_read_encoder(ENCODER_R);
}



static int16_t calculate_error(uint16_t first_distance, uint16_t second_distance, float gain)
{
  if (first_distance > OUT_OF_BOUNDS_VALUE || second_distance > OUT_OF_BOUNDS_VALUE)
    {
      return(0);
    }
  int16_t error = (int16_t)(gain*((int16_t)first_distance - second_distance));
  return(error);
}

void motor_disable_all(void)
{
  motor_disable(MOTOR_L);
  motor_disable(MOTOR_R);
}

void motor_enable_all(void)
{
  motor_enable(MOTOR_L);
  motor_enable(MOTOR_R);
}

static void motor_enable(motor_t motor)
{
  HAL_TIM_PWM_Start(motor_handles[motor], TIM_CHANNEL_1);
}

static void motor_disable(motor_t motor)
{
  HAL_TIM_PWM_Stop(motor_handles[motor], TIM_CHANNEL_1);
}

static void motor_set_forward(motor_t motor)
{
  if(motor == MOTOR_L)
    {
      HAL_GPIO_WritePin(LEFT_MOTOR_REVERSE_GPIO_Port, LEFT_MOTOR_REVERSE_Pin, LEFT_MOTOR_FORWARD);
    }
  else if (motor == MOTOR_R)
    {
      HAL_GPIO_WritePin(RIGHT_MOTOR_REVERSE_GPIO_Port, RIGHT_MOTOR_REVERSE_Pin, RIGHT_MOTOR_FORWARD);
    }
}
static void motor_set_reverse(motor_t motor)
{
  if (motor == MOTOR_L)
    {
      HAL_GPIO_WritePin(LEFT_MOTOR_REVERSE_GPIO_Port, LEFT_MOTOR_REVERSE_Pin, LEFT_MOTOR_REVERSE);
    }
  else if (motor == MOTOR_R)
    {
      HAL_GPIO_WritePin(RIGHT_MOTOR_REVERSE_GPIO_Port, RIGHT_MOTOR_REVERSE_Pin, RIGHT_MOTOR_REVERSE);
    }
}


