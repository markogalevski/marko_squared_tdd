
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

TEST(StateMachine, FinishingLeftTurnResetsStateToForward)
{
  LEFT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  POINTERS_EQUAL(sm_forward, markobot->state_method);
}

TEST(StateMachine, FinishingRightTurnResetsStateToForward)
{
  RIGHT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  POINTERS_EQUAL(sm_forward, markobot->state_method);
}

TEST(StateMachine, FinishingUTurnResetsStateToForward)
{
  FRONT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  POINTERS_EQUAL(sm_forward, markobot->state_method);
}

TEST(StateMachine, AlwaysMeasureStateEveryTimerInterrupt)
{

  MEASURE_TIMER_INTERRUPT();
  LEFT_SENSOR_INTERRUPT();
  RIGHT_SENSOR_INTERRUPT();
  FRONT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_MEASURE, markobot->current_state);
  POINTERS_EQUAL(sm_mapping_measure, markobot->state_method);
}

TEST(StateMachine, MeasurementTransitionsIntoDeadReckoning)
{
  MEASURE_TIMER_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_DEAD_RECKONING, markobot->current_state);
  POINTERS_EQUAL(sm_dead_reckoning, markobot->state_method);
}

TEST(StateMachine, DeadReckoningTransitionsIntoMovingForward)
{
  MEASURE_TIMER_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  POINTERS_EQUAL(sm_forward, markobot->state_method);
}

TEST(StateMachine, FloorInterruptStops)
{
  FLOOR_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_STOP, markobot->current_state);
  POINTERS_EQUAL(sm_stop, markobot->state_method);
}

TEST(StateMachine, FloorInterruptIsKing)
{
  MEASURE_TIMER_INTERRUPT();
  FLOOR_SENSOR_INTERRUPT();
  LEFT_SENSOR_INTERRUPT();
  RIGHT_SENSOR_INTERRUPT();
  FRONT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_STOP, markobot->current_state);
  POINTERS_EQUAL(sm_stop, markobot->state_method);
}

TEST(StateMachine, SecondButtonPressTriggersSolvingAlgorithm)
{
  FLOOR_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_STOP, markobot->current_state);
  POINTERS_EQUAL(sm_stop, markobot->state_method);
  BUTTON_PRESS();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_SOLVING, markobot->current_state);
  POINTERS_EQUAL(sm_solving, markobot->state_method);
}
TEST(StateMachine, SolvingAlgorithmTransitionsIntoIdle)
{
  FLOOR_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  BUTTON_PRESS();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_SOLVING_COMPLETE, markobot->current_state);
  POINTERS_EQUAL(sm_solving_complete, markobot->state_method);
}

TEST(StateMachine, ThirdButtonPressTriggersRacingStates)
{
  markobot->next_state = STATE_SOLVING_COMPLETE;
  sm_state_transition(markobot);
  BUTTON_PRESS();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_RACING, markobot->current_state);
  POINTERS_EQUAL(sm_racing, markobot->state_method);
}

