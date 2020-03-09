#ifndef _MOTOR_CONTROLLERS_H
#define _MOTOR_CONTROLLERS_H
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "pinout.h"

typedef enum
{
  MOTOR_L,
  MOTOR_R,
  NUM_MOTORS
}motor_t;

typedef enum
{
	ENCODER_L,
	ENCODER_R,
	NUM_ENCODERS
}encoder_t;

typedef enum
{
  LEFT_90_DEG,
  RIGHT_90_DEG,
  LEFT_180_DEG,
  NUM_IN_PLACE
}in_place_t;

#define WHEEL_RADIUS_CM		(6)
#define WHEEL_CIRCUM_CM		(float)(2*WHEEL_RADIUS_CM*3.1415)
#define ENCODER_FULL_REV	(200)	//TO BE TESTED
#define CM_TO_TICKS(value)	( ((float)value)/WHEEL_CIRCUM_CM * ENCODER_FULL_REV)
#define TICKS_TO_CM(value)	((uint32_t) ((float) value * WHEEL_CIRCUM_CM) / ENCODER_FULL_REV)

#define CELL_SIZE_CM		(13)
#define CELL_SIZE_TICKS		((uint32_t) CM_TO_TICKS(CELL_SIZE_CM))
#define ENCODER_360_TURN 	(120) 	//TO BE TESTED
#define ENCODER_180_TURN 	(ENCODER_360_TURN/2)
#define ENCODER_90_TURN 	(ENCODER_180_TURN/2)

#define RIGHT_MOTOR_FORWARD	GPIO_PIN_RESET
#define RIGHT_MOTOR_REVERSE	GPIO_PIN_SET
#define LEFT_MOTOR_FORWARD	GPIO_PIN_RESET
#define LEFT_MOTOR_REVERSE	GPIO_PIN_SET

#define OUT_OF_BOUNDS_VALUE		130
#define CENTRING_P_GAIN		(float) 0.5
#define CURVE_P_GAIN		(float) 0.5

extern TIM_HandleTypeDef htim2, htim3, htim4, htim5;

void motor_disable_all(void);
void motor_enable_all(void);
void motor_controller_forward(uint16_t right_data, uint16_t left_data, float gain);
void motor_controller_in_place(in_place_t direction);
void motor_controller_coast(float coast_cm);
uint32_t motor_read_reference_encoder(encoder_t encoder);

extern uint32_t (*motor_read_encoder)(encoder_t encoder);
void motor_reset_reference_encoder_values(void);
extern void (*update_pwm_value)(TIM_HandleTypeDef *htim, int32_t error);

#endif

