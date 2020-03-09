#include "boolean_utils.h"

bool isStoppingNext(robot_t *robot)
{
 return ((robot->next_state == STATE_STOP));
}

bool isNotTurning(robot_t *robot)
{
  return (robot->current_state == STATE_FORWARD
	    || robot->current_state == STATE_DEAD_RECKONING
	    || robot->current_state == STATE_MEASURE
	    );
}

bool isNotTurningLeftNext(robot_t *robot)
{
  return(robot->next_state != STATE_TURNING_LEFT);
}

bool isNotTurningRightNext(robot_t *robot)
{
  return(robot->next_state != STATE_TURNING_RIGHT);
}

bool hasTurnsScheduled(robot_t *robot)
{
  return (!(robot->next_state != STATE_TURNING_LEFT
	    && robot->next_state != STATE_TURNING_RIGHT
	    && robot->next_state != STATE_TURNING_AROUND));
}
