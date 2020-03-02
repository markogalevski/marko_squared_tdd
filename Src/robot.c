#include "stm32f4xx_hal.h"
#include "robot.h"
#include "pinout.h"


TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

static volatile uint32_t *const encoder_contents[NUM_ENCODERS] =
{
	(uint32_t *) TIM3_BASE + 0x24UL, (uint32_t *) TIM4_BASE + 0x24UL
};


static void sm_solving(robot_t *robot);

static void sm_solving_complete(robot_t *robot);

static void sm_stop(robot_t *robot);

static void sm_racing(robot_t *robot);

static void sm_mapping_measure(robot_t *robot);

static void sm_dead_reckoning(robot_t *robot);

static void robot_orientation_incr_cw(robot_t *);
static void robot_orientation_incr_ccw(robot_t *);
static uint32_t robot_read_encoder(encoder_t encoder);
static uint32_t robot_convert_encoder_data(robot_t * robot, uint32_t currLeftData, uint32_t currRightData);

static uint32_t referenceLeftEncoder = 0;
static uint32_t referenceRightEncoder = 0;

static void reset_reference_encoder_values(void);

static state_method_t state_methods[NUM_STATES] =
{
	(state_method_t) sm_forward, (state_method_t) sm_turning_left,
	(state_method_t) sm_turning_right, (state_method_t) sm_turning_around,
	(state_method_t) sm_solving, (state_method_t) sm_solving_complete,
	(state_method_t) sm_stop, (state_method_t) sm_racing,
	(state_method_t) sm_mapping_measure, (state_method_t) sm_dead_reckoning,
	(state_method_t) sm_power_on
};

robot_t *robot_create(void)
{
  robot_t *robot = (robot_t *)malloc(sizeof(robot_t));
  robot->current_state = STATE_POWER_ON;
  robot->orientation = NORTH;
  robot->x_location = 0;
  robot->y_location = 0;
  robot->state_method = sm_power_on;

  return(robot);
}

void robot_run(robot_t *robot)
{
	robot->state_method(robot);
}

void robot_destroy(robot_t *robot)
{
  free(robot);
}

void sm_state_transition(robot_t *robot)
{
	robot->current_state = robot->next_state;
	robot->state_method = state_methods[robot->current_state];
}

void sm_power_on(robot_t *robot)
{

}
void sm_forward(robot_t *robot)
{
	HAL_GPIO_WritePin(RIGHT_MOTOR_REVERSE_GPIO_Port, RIGHT_MOTOR_REVERSE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT_MOTOR_REVERSE_GPIO_Port, LEFT_MOTOR_REVERSE_Pin, GPIO_PIN_RESET);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void sm_turning_left(robot_t *robot)
{
	NVIC_DisableIRQ(I2C2_EV_IRQn);

	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	uint32_t rightMotor_start = robot_read_encoder(ENCODER_R);
	uint32_t rightMotor_current = rightMotor_start;
	HAL_GPIO_WritePin(LEFT_MOTOR_REVERSE_GPIO_Port, LEFT_MOTOR_REVERSE_Pin, GPIO_PIN_SET);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	while(rightMotor_current - rightMotor_start < ENCODER_90_TURN)
	{
		rightMotor_current = robot_read_encoder(ENCODER_R);
	}
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	robot_orientation_incr_ccw(robot);
	if (robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_FORWARD;
	}
	reset_reference_encoder_values();
	NVIC_EnableIRQ(I2C2_EV_IRQn);
}

void sm_turning_right(robot_t *robot)
{
	NVIC_DisableIRQ(I2C2_EV_IRQn);

	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	uint32_t leftMotor_start = robot_read_encoder(ENCODER_L);
	uint32_t leftMotor_current = leftMotor_start;
	HAL_GPIO_WritePin(RIGHT_MOTOR_REVERSE_GPIO_Port, RIGHT_MOTOR_REVERSE_Pin, GPIO_PIN_SET);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	while(leftMotor_current - leftMotor_start < ENCODER_90_TURN)
	{
		leftMotor_current = robot_read_encoder(ENCODER_L);
	}
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	robot_orientation_incr_cw(robot);
	if (robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_FORWARD;
	}
	reset_reference_encoder_values();

	NVIC_EnableIRQ(I2C2_EV_IRQn);

}

void sm_turning_around(robot_t *robot)
{
	NVIC_DisableIRQ(I2C2_EV_IRQn);

	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	uint32_t rightMotor_start = robot_read_encoder(ENCODER_R);
	uint32_t rightMotor_current = rightMotor_start;
	HAL_GPIO_WritePin(LEFT_MOTOR_REVERSE_GPIO_Port, LEFT_MOTOR_REVERSE_Pin, GPIO_PIN_SET);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	while(rightMotor_current - rightMotor_start < ENCODER_180_TURN)
	{
		rightMotor_current = robot_read_encoder(ENCODER_R);
	}
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	robot_orientation_incr_ccw(robot);
	robot_orientation_incr_ccw(robot);
	if(robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_FORWARD;
	}
	reset_reference_encoder_values();
	NVIC_EnableIRQ(I2C2_EV_IRQn);

}

static void sm_solving(robot_t *robot)
{
	/**
	 * Here's the entire maze solving algorithm. It should probably call a function defined in an external file
	 */
}

static void sm_solving_complete(robot_t *robot)
{
	HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_SET);
}

static void sm_stop(robot_t *robot)
{
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	HAL_GPIO_TogglePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin);
	HAL_Delay(200);
}

static void sm_racing(robot_t *robot)
{

}

static void sm_dead_reckoning(robot_t *robot)
{
	uint32_t currRightEncoder = robot_read_encoder(ENCODER_R);
	uint32_t differenceRight = currRightEncoder - referenceRightEncoder;
	uint32_t cell_diff = CONVERT_TO_CELL(differenceRight);
	if (robot->orientation == NORTH)
	{
		robot->y_location += cell_diff;
	}
	else if (robot->orientation == SOUTH)
	{
		robot->y_location -= cell_diff;
	}
	else if (robot->orientation == EAST)
	{
		robot->x_location += cell_diff;
	}
	else
	{
		robot->x_location -= cell_diff;
	}

	if (robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_MEASURE;
	}

}

static void sm_mapping_measure(robot_t *robot)
{
	/**
	 * Sensor Reading Goes Here
	 */
	/**
	 * Place data in correct matrix location
	 */

	robot->next_state = STATE_FORWARD;
}

static uint32_t robot_convert_encoder_data(robot_t * robot, uint32_t currLeftData, uint32_t currRightData)
{
  static uint32_t prevLeftData = 0;
  static uint32_t prevRightData = 0;
  return(0);
}

static uint32_t robot_read_encoder(encoder_t encoder)
{
	return *encoder_contents[encoder];
}

static void reset_reference_encoder_values(void)
{
	referenceLeftEncoder = robot_read_encoder(ENCODER_L);
	referenceRightEncoder = robot_read_encoder(ENCODER_R);
}

static void robot_orientation_incr_cw(robot_t *robot)
{
	robot->orientation = (robot->orientation + 1)%(NUM_ORIENTATIONS);
}

static void robot_orientation_incr_ccw(robot_t *robot)
{
	if (--robot->orientation < 0)
		{
		robot->orientation = WEST;
		}
}
