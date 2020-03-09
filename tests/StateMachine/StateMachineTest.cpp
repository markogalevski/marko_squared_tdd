extern "C"
{
#include "robot.h"
#include "main.h"
#include "motor_controller_spy.h"
#include "sensors_fake.h"
}
#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"


extern robot_t *markobot;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

extern void MX_TIM2_Init(void);

TEST_GROUP(StateMachine)
{

  void setup()
  {
    motor_reset_reference_encoder_values();
    motor_clear_fake_encoders();
    markobot = robot_create();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_GPIO_Init();
    UT_PTR_SET(motor_read_encoder, motor_read_encoder_fake);
    UT_PTR_SET(mapping_create_node, mapping_create_node_fake);
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
  robot_cleanup();
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

TEST(StateMachine, FinishLeftTurnUpdatesOrientation)
{
  LEFT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(WEST, markobot->orientation);
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

TEST(StateMachine, FinishingRightTurnUpdatesOrientation)
{
  RIGHT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(EAST, markobot->orientation);
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

TEST(StateMachine, FinishingUTurnUpdatesOrientation)
{
  FRONT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(SOUTH, markobot->orientation);
}

TEST(StateMachine, DeadReckoningLosesToMovementInterrupts)
{

  MEASURE_TIMER_INTERRUPT();
  LEFT_SENSOR_INTERRUPT();
  RIGHT_SENSOR_INTERRUPT();
  FRONT_SENSOR_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_TURNING_LEFT, markobot->current_state);
  POINTERS_EQUAL(sm_turning_left, markobot->state_method);
}

TEST(StateMachine, TimerInterruptStartsDeadReckoning)
{
  MEASURE_TIMER_INTERRUPT();
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_DEAD_RECKONING, markobot->current_state);
  POINTERS_EQUAL(sm_dead_reckoning, markobot->state_method);
}

TEST(StateMachine, DeadReckoningTransitionsIntoMeasurementIfPositionUpdated)
{
  MEASURE_TIMER_INTERRUPT();
  sm_state_transition(markobot);
  uint32_t current_encoder = motor_read_encoder(ENCODER_R);
  uint32_t start_encoder = current_encoder;

  while(current_encoder - start_encoder < CELL_SIZE_TICKS)
    {
      current_encoder = motor_read_encoder(ENCODER_R);
    }

  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_MEASURE, markobot->current_state);
  POINTERS_EQUAL(sm_mapping_measure, markobot->state_method);
}

TEST(StateMachine, MeasurementTransitionsIntoForward)
{
  markobot->next_state = STATE_MEASURE;
  robot_run(markobot);
  sm_state_transition(markobot);
  robot_run(markobot);
  sm_state_transition(markobot);
  LONGS_EQUAL(STATE_FORWARD, markobot->current_state);
  LONGS_EQUAL(sm_forward, markobot->state_method);
}

TEST(StateMachine, DeadReckoningTransitionsIntoMovingForwardIfNoUpdate)
{
  MEASURE_TIMER_INTERRUPT();
  uint32_t current_encoder;
  for (int i = 0; i < 2; i++)
    {
      current_encoder = motor_read_encoder(ENCODER_R);
    }
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

