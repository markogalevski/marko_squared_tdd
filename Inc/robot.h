#ifndef _ROBOT_H
#define _ROBOT_H

#include "states.h"
#include <stdint.h>

#define ENCODER_STEPS 20	//TO BEST TESTED
#define ENCODER_360_TURN 55 //TO BE TESTED
#define ENCODER_180_TURN ENCODER_360_TURN/2
#define ENCODER_90_TURN ENCODER_180_TURN/2

#define CONVERT_TO_CELL(diff) (diff)

typedef enum
{
	NORTH,
	SOUTH,
	EAST,
	WEST,
	NUM_ORIENTATIONS
}orientation_t;

typedef enum
{
	ENCODER_L,
	ENCODER_R,
	NUM_ENCODERS
}encoder_t;

typedef struct robot_t robot_t;

typedef void (*state_method_t)(robot_t *);

typedef struct robot_t
{
	states_t current_state;
	states_t next_state;
	uint8_t x_location;
	uint8_t y_location;
	state_method_t state_method;
	orientation_t orientation;
}robot_t;


robot_t *robot_create(void);
void robot_run(robot_t *robot);
void robot_destroy(robot_t *robot);

void sm_state_transition(robot_t *robot);
void sm_power_on(robot_t *robot);
void sm_forward(robot_t *robot);
void sm_turning_left(robot_t *robot);
void sm_turning_right(robot_t *robot);
void sm_turning_around(robot_t *robot);
void sm_mapping_measure(robot_t *robot);
void sm_dead_reckoning(robot_t *robot);
void sm_stop(robot_t *robot);
void sm_solving(robot_t *robot);
void sm_solving_complete(robot_t *robot);
void sm_racing(robot_t *robot);


#endif
