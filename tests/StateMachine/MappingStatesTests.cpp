#ifndef EXTERN
#define EXTERN
#endif

#include "main.h"

#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"

extern robot_t *markobot;
TIM_HandleTypeDef htim2, htim3, htim4, htim5;

TEST_GROUP(MappingForward)
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


TEST(MappingForward, StraightLineCorrection)
{
  robot_run(markobot);

}
