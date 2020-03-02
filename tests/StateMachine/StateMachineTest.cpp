
#define EXTERN

#include "main.h"
#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"

robot_t *markobot;

TEST_GROUP(StateMachine)
{

  void setup()
  {
    markobot = robot_create();
    BUTTON_PRESS();
    sm_state_transition(markobot);
  }

  void teardown()
  {
   robot_destroy(markobot);
  }
};

TEST(StateMachine, InitCreatesEmptyBot)
{
  robot_destroy(markobot);
  markobot = robot_create();
  LONGS_EQUAL(NORTH, markobot->orientation);
  LONGS_EQUAL(0, markobot->x_location);
  LONGS_EQUAL(0, markobot->y_location);
  POINTERS_EQUAL(sm_power_on, markobot->state_method);
  LONGS_EQUAL(STATE_POWER_ON, markobot->current_state);
}

TEST(StateMachine, ButtonTransitionsPowerOnToMappingForward)
{

  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  POINTERS_EQUAL(sm_forward, markobot->state_method);
}

TEST(StateMachine, EmptySpaceLeftTriggersLeftTurn)
{
  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  LEFT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_TURNING_LEFT, markobot->current_state);
  POINTERS_EQUAL(sm_turning_left, markobot->state_method);
}

TEST(StateMachine, EmptySpaceRightTriggersRightTurn)
{
  RIGHT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_TURNING_RIGHT, markobot->current_state);
  POINTERS_EQUAL(sm_turning_right, markobot->state_method);
}

TEST(StateMachine, LeftTurnTakesPrecedenceOverRightTurn)
{
  LEFT_SENSOR_INTERRUPT();
  RIGHT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_TURNING_LEFT, markobot->current_state);
  POINTERS_EQUAL(sm_turning_left, markobot->state_method);
}

TEST(StateMachine, TIntersectionPrefersLeft)
{
  FRONT_SENSOR_INTERRUPT();
  RIGHT_SENSOR_INTERRUPT();
  LEFT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_TURNING_LEFT, markobot->current_state);
  POINTERS_EQUAL(sm_turning_left, markobot->state_method);
}

TEST(StateMachine, DeadEndResultsInTurnAround)
{
  FRONT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_TURNING_AROUND, markobot->current_state);
  POINTERS_EQUAL(sm_turning_around, markobot->state_method);
}
