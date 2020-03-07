#ifndef _ROBOT_H
#define _ROBOT_H

#include "states.h"
#include <stdint.h>
#include "mapping.h"



#define CONVERT_TO_CELL(diff) (diff)

typedef enum
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NUM_ORIENTATIONS
}orientation_t;



typedef struct robot_t robot_t;

typedef void (*state_method_t)(robot_t *);

typedef struct robot_t
{
	states_t current_state;
	states_t next_state;
	int8_t x_location;
	int8_t y_location;
	state_method_t state_method;
	orientation_t orientation;
	bool walls[NUM_ORIENTATIONS];
}robot_t;


robot_t *robot_create(void);
void robot_run(robot_t *robot);
void robot_destroy(robot_t *robot);
void robot_cleanup(void);
void Hardware_Setup(void);
#define TESTING_MODE

#ifdef TESTING_MODE
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
int motor_control_centring(uint16_t left_data, uint16_t right_data, float gain);
#endif

#endif
