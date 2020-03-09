#ifndef _BOOL_UTILS_H
#define _BOOL_UTILS_H

#include <stdbool.h>
#include "robot.h"

bool isStoppingNext(robot_t *robot);
bool isNotTurning(robot_t *robot);
bool isNotTurningLeftNext(robot_t *robot);
bool isNotTurningRightNext(robot_t *robot);
bool hasTurnsScheduled(robot_t *robot);

#endif
