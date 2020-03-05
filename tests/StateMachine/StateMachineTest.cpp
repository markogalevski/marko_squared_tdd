extern "C"
{
#include "robot.h"
#include "main.h"
}
#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"


robot_t *markobot;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

static void MX_TIM2_Init(void);

TEST_GROUP(StateMachine)
{

  void setup()
  {
    markobot = robot_create();
    MX_TIM2_Init();
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


static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2400;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler("HAL_TIM_Base_Init");
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler("HAL_TIM_ConfigClockSource");
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler("HAL_TIM_PWM_Init");
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler("HAL_TIMEx_MasterConfigSynchronization");
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 799;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler("HAL_TIM_PWM_ConfigChannel");
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

void Error_Handler(char *msg)
{
  printf("Error Called: %s", msg);
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspPostInit 0 */

  /* USER CODE END TIM2_MspPostInit 0 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA5     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = LEFT_MOTOR_PWM_PIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(LEFT_MOTOR_PWM_PIN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspPostInit 1 */

  /* USER CODE END TIM2_MspPostInit 1 */
  }
  else if(htim->Instance==TIM5)
  {
  /* USER CODE BEGIN TIM5_MspPostInit 0 */

  /* USER CODE END TIM5_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM5 GPIO Configuration
    PA0-WKUP     ------> TIM5_CH1
    */
    GPIO_InitStruct.Pin = RIGHT_MOTOR_PWM_PIN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
    HAL_GPIO_Init(RIGHT_MOTOR_PWM_PIN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM5_MspPostInit 1 */

  /* USER CODE END TIM5_MspPostInit 1 */
  }

}
