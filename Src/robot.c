#define EXTERN_FLAG
#include "robot.h"
#include "sensors.h"
#include "main.h"
#include "motor_controllers.h"
#include "pinout.h"

#ifdef TESTING_MODE
#define STATIC
#else
#define STATIC static
#endif

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

node_t *previousNode;

static void robot_orientation_incr_cw(robot_t *);
static void robot_orientation_incr_ccw(robot_t *);

static orientation_t increment_orientation_cw(orientation_t orientation);
static orientation_t increment_orientation_ccw(orientation_t orientation);
static void refresh_walls(robot_t * robot);
static void update_position(robot_t *robot);

robot_t *robot_create(void)
{
  robot_t *robot = (robot_t *)malloc(sizeof(robot_t));
  robot->current_state = STATE_POWER_ON;
  robot->orientation = NORTH;
  robot->x_location = 0;
  robot->y_location = 0;
  robot->state_method = sm_power_on;
  for (int i = 0; i < NUM_ORIENTATIONS; i++)
    {
      robot->walls[i] = false;
    }
  return(robot);
  previousNode = (node_t *)malloc(sizeof(node_t));
  previousNode->prev = NULL;
}

void robot_run(robot_t *robot)
{
	robot->state_method(robot);
}

void robot_destroy(robot_t *robot)
{
  free(robot);
}

void robot_cleanup(void)
{
  node_t *current_target;
/*  if (previousNode->prev != NULL)
    {
      current_target = previousNode->prev;
    }
 /*node_t *next_target;
  while(next_target != NULL)
    {
      next_target = current_target->prev;
      if (current_target != NULL)
	{
	  free(current_target);
	}
      current_target = next_target;
    }
  previousNode = NULL;
*/
}


STATIC void sm_power_on(robot_t *robot)
{
      motor_disable_all();
      HAL_GPIO_TogglePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin);
      HAL_Delay(500);
}

STATIC void sm_forward(robot_t *robot)
{
	uint16_t left_sensor_data, right_sensor_data;
	sensor_read(LEFT_SENSOR, &left_sensor_data);
	sensor_read(RIGHT_SENSOR, &right_sensor_data);
	motor_controller_forward(right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
}

STATIC void sm_turning_left(robot_t *robot)
{

	motor_controller_coast(7.0);
	update_position(robot);
	previousNode = mapping_create_node(robot->x_location, robot->y_location, (cell_t *) &robot->walls, previousNode);
	refresh_walls(robot);
	motor_controller_in_place(LEFT_90_DEG);

	robot_orientation_incr_ccw(robot);
	motor_reset_reference_encoder_values();
	motor_controller_coast(7.0);

	if (robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_FORWARD;
	}

}

STATIC void sm_turning_right(robot_t *robot)
{

	motor_controller_coast(7.0);
	update_position(robot);
	previousNode = mapping_create_node(robot->x_location, robot->y_location, (cell_t *)&robot->walls, previousNode);
	refresh_walls(robot);
	motor_controller_in_place(RIGHT_90_DEG);

	robot_orientation_incr_cw(robot);
	motor_reset_reference_encoder_values();
	motor_controller_coast(7.0);

	if (robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_FORWARD;
	}

}

STATIC void sm_turning_around(robot_t *robot)
{

	motor_controller_coast(7.0);
	motor_controller_in_place(LEFT_180_DEG);
	robot_orientation_incr_ccw(robot);
	robot_orientation_incr_ccw(robot);
	motor_reset_reference_encoder_values();

	if(robot->next_state != STATE_STOP)
	{
		robot->next_state = STATE_FORWARD;
	}

}



STATIC void sm_mapping_measure(robot_t *robot)
{
	uint16_t left_data, right_data, front_data;
	uint16_t orientation_helper;
	sensor_read(LEFT_SENSOR, &left_data);
	sensor_read(RIGHT_SENSOR, &right_data);
	sensor_read(FRONT_SENSOR, &front_data);
	if(left_data < WALL_THRESHOLD)
	  {
	    orientation_helper = (uint16_t) increment_orientation_ccw(robot->orientation);
	    robot->walls[orientation_helper] = true;
	  }
	if (right_data < WALL_THRESHOLD)
	  {
	    orientation_helper = (uint16_t) increment_orientation_cw(robot->orientation);
	    robot->walls[orientation_helper] = true;
	  }
	if (front_data < WALL_THRESHOLD)
	  {
	    orientation_helper = (uint16_t) robot->orientation;
	    robot->walls[orientation_helper] = true;
	  }

	if (	robot->next_state != STATE_STOP
	    && robot->next_state != STATE_TURNING_LEFT
	    && robot->next_state != STATE_TURNING_RIGHT
	    && robot->next_state != STATE_TURNING_AROUND)
	  {
	    robot->next_state = STATE_DEAD_RECKONING;
	  }
}

STATIC void sm_dead_reckoning(robot_t *robot)
{

	uint32_t current_right_encoder = motor_read_encoder(ENCODER_R);
	uint32_t difference_right = current_right_encoder - motor_read_reference_encoder(ENCODER_R);
	if (TICKS_TO_CM(difference_right) - (float)CELL_SIZE_CM < 0.01)
	  {
	    update_position(robot);
	    previousNode = mapping_create_node(robot->x_location, robot->y_location, (cell_t *)&robot->walls, previousNode);


	  }

	if (	robot->next_state != STATE_STOP
	    && robot->next_state != STATE_TURNING_LEFT
	    && robot->next_state != STATE_TURNING_RIGHT
	    && robot->next_state != STATE_TURNING_AROUND)
	{
		robot->next_state = STATE_FORWARD;
	}

}



STATIC void sm_stop(robot_t *robot)
{
	motor_disable_all();
	HAL_GPIO_TogglePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin);
	HAL_Delay(200);
}

STATIC void sm_solving(robot_t *robot)
{
	/**
	 * Here's the entire maze solving algorithm. It should call a function defined in an external file
	 */
    robot->next_state = STATE_SOLVING_COMPLETE;
}

STATIC void sm_solving_complete(robot_t *robot)
{
	HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_SET);
}


STATIC void sm_racing(robot_t *robot)
{

}

static state_method_t state_methods[NUM_STATES] =
{
	(state_method_t) sm_forward, (state_method_t) sm_turning_left,
	(state_method_t) sm_turning_right, (state_method_t) sm_turning_around,
	(state_method_t) sm_solving, (state_method_t) sm_solving_complete,
	(state_method_t) sm_stop, (state_method_t) sm_racing,
	(state_method_t) sm_mapping_measure, (state_method_t) sm_dead_reckoning,
	(state_method_t) sm_power_on
};


void sm_state_transition(robot_t *robot)
{
	robot->current_state = robot->next_state;
	robot->state_method = state_methods[robot->current_state];
}





static void robot_orientation_incr_cw(robot_t *robot)
{
	robot->orientation = increment_orientation_cw(robot->orientation);
}


static void robot_orientation_incr_ccw(robot_t *robot)
{
  robot->orientation = increment_orientation_ccw(robot->orientation);

}

static orientation_t increment_orientation_ccw(orientation_t orientation)
{
  if (orientation == NORTH)
    {
      return(WEST);
    }
  else
    {
      return(--orientation);
    }
}

static orientation_t increment_orientation_cw(orientation_t orientation)
{
  return((++orientation)%(NUM_ORIENTATIONS));

}

static void refresh_walls(robot_t * robot)
{
  for (int i = 0; i < NUM_ORIENTATIONS;i++)
    {
      robot->walls[i] = false;
    }
}

static void update_position(robot_t *robot)
{
  if (robot->orientation == NORTH)
    {
      robot->y_location += 1;
    }
  else if (robot->orientation == EAST)
    {
      robot->x_location += 1;
    }
  else if (robot->orientation == SOUTH)
    {
      robot->y_location -= 1;
    }
  else if (robot->orientation == WEST)
    {
      robot->x_location -= 1;
    }
}
