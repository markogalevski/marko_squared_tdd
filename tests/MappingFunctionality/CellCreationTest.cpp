
extern "C"
{
#include "pinout.h"
#include "robot.h"
#include "sensors_fake.h"
#include "main.h"
}
#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"

extern robot_t *markobot;
extern uint16_t left_sensor_data, right_sensor_data, front_sensor_data;

TEST_GROUP(MappingFunctionality)
{
  void setup()
  {
    markobot = robot_create();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_GPIO_Init();
    UT_PTR_SET(sensor_read, sensor_read_stub);
    UT_PTR_SET(mapping_create_node, mapping_create_node_fake);
    BUTTON_PRESS();
    sm_state_transition(markobot);
  }
  void teardown()
  {

    robot_destroy(markobot);
  }
};

TEST(MappingFunctionality, RobotInitsWithClearWalls)
{
  for (int i = 0; i < NUM_ORIENTATIONS; i++)
    {
      LONGS_EQUAL(0, markobot->walls[i]);
    }
}

TEST(MappingFunctionality, RobotIdentifiesNoWalls)
{
  sensor_inject_fake_data(LEFT_SENSOR, 155);
  sensor_inject_fake_data(RIGHT_SENSOR, 155);
  sensor_inject_fake_data(FRONT_SENSOR, 155);
  sm_mapping_measure(markobot);
  for (int i = 0; i < NUM_ORIENTATIONS; i++)
    {
      LONGS_EQUAL(0, markobot->walls[i]);
    }

}

TEST(MappingFunctionality, RobotIdentifiesWallToLeftAtStart)
{
  sensor_inject_fake_data(LEFT_SENSOR, 30);
  sensor_inject_fake_data(RIGHT_SENSOR, 155);
  sensor_inject_fake_data(FRONT_SENSOR, 155);
  sm_mapping_measure(markobot);
  LONGS_EQUAL(0, markobot->walls[EAST]);
  LONGS_EQUAL(0, markobot->walls[NORTH]);
  LONGS_EQUAL(0, markobot->walls[SOUTH]);
  LONGS_EQUAL(1, markobot->walls[WEST]);

}

TEST(MappingFunctionality, RobotIdentifiesWallStraightAheadAtStart)
{
  sensor_inject_fake_data(LEFT_SENSOR, 150);
  sensor_inject_fake_data(RIGHT_SENSOR, 155);
  sensor_inject_fake_data(FRONT_SENSOR, 30);
  sm_mapping_measure(markobot);
  LONGS_EQUAL(0, markobot->walls[EAST]);
  LONGS_EQUAL(1, markobot->walls[NORTH]);
  LONGS_EQUAL(0, markobot->walls[SOUTH]);
  LONGS_EQUAL(0, markobot->walls[WEST]);
}

TEST(MappingFunctionality, RobotIdentifiesSWallToRightAtStart)
{
  sensor_inject_fake_data(LEFT_SENSOR, 150);
  sensor_inject_fake_data(RIGHT_SENSOR, 30);
  sensor_inject_fake_data(FRONT_SENSOR, 155);
  sm_mapping_measure(markobot);
  LONGS_EQUAL(1, markobot->walls[EAST]);
  LONGS_EQUAL(0, markobot->walls[NORTH]);
  LONGS_EQUAL(0, markobot->walls[SOUTH]);
  LONGS_EQUAL(0, markobot->walls[WEST]);
}

TEST(MappingFunctionality, RobotCorrectlyPlacesWallsAfter90DegRotation)
{
  markobot->orientation = WEST;

  sensor_inject_fake_data(LEFT_SENSOR, 150);
  sensor_inject_fake_data(RIGHT_SENSOR, 30);
  sensor_inject_fake_data(FRONT_SENSOR, 155);
  markobot->next_state = STATE_MEASURE;
  sm_state_transition(markobot);
  robot_run(markobot);
  LONGS_EQUAL(0, markobot->walls[EAST]);
  LONGS_EQUAL(1, markobot->walls[NORTH]);
  LONGS_EQUAL(0, markobot->walls[SOUTH]);
  LONGS_EQUAL(0, markobot->walls[WEST]);
}

TEST(MappingFunctionality, RobotCorrectlyPlacesWallsAfter90DegRotation2)
{
  markobot->orientation = EAST;

  sensor_inject_fake_data(LEFT_SENSOR, 150);
  sensor_inject_fake_data(RIGHT_SENSOR, 30);
  sensor_inject_fake_data(FRONT_SENSOR, 155);
  markobot->next_state = STATE_MEASURE;
  sm_state_transition(markobot);
  robot_run(markobot);
  LONGS_EQUAL(0, markobot->walls[EAST]);
  LONGS_EQUAL(0, markobot->walls[NORTH]);
  LONGS_EQUAL(1, markobot->walls[SOUTH]);
  LONGS_EQUAL(0, markobot->walls[WEST]);
}

TEST(MappingFunctionality, RobotCorrectlyPlacesWallsAfter180DegRotation)
{
  markobot->orientation = SOUTH;
  sensor_inject_fake_data(LEFT_SENSOR, 30);
  sensor_inject_fake_data(RIGHT_SENSOR, 155);
  sensor_inject_fake_data(FRONT_SENSOR, 30);
  markobot->next_state = STATE_MEASURE;
  sm_state_transition(markobot);
  robot_run(markobot);
  LONGS_EQUAL(1, markobot->walls[EAST]);
  LONGS_EQUAL(0, markobot->walls[NORTH]);
  LONGS_EQUAL(1, markobot->walls[SOUTH]);
  LONGS_EQUAL(0, markobot->walls[WEST]);
}

TEST_GROUP(NodeCleaning)
{
  void setup()
  {
   markobot = robot_create();
  }
  void teardown()
  {
    robot_destroy(markobot);
  }
};

extern node_t *newestNode;
TEST_GROUP(MapGeneration)
{
  void setup()
  {
    markobot = robot_create();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_GPIO_Init();
    UT_PTR_SET(sensor_read, sensor_read_stub);
    BUTTON_PRESS();
    sm_state_transition(markobot);
  }
   void teardown()
   {
     robot_destroy(markobot);

   }
};

TEST(MapGeneration, NoFaultOnEmptyList)
{

}

TEST(MapGeneration, DeletesSingleValidElement)
{
  markobot->walls[SOUTH] = true;
  newestNode = mapping_create_node(1, 1, (cell_t *) &markobot->walls, newestNode);
}

TEST(MapGeneration, DeletesChainOfTwoElements)
{
  markobot->walls[SOUTH] = true;
  newestNode = mapping_create_node(1, 1, (cell_t *) &markobot->walls, newestNode);
  markobot->walls[SOUTH] = false;
  markobot->walls[EAST] = true;
  markobot->walls[WEST] = true;
  newestNode = mapping_create_node(1, 0, (cell_t *) &markobot->walls, newestNode);
}

