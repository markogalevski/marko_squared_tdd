extern "C"
{
#include "pinout.h"
#include "robot.h"
#include "FakeSensors.h"
#include "main.h"
#include "motor_controller_spy.h"
}
#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"

extern robot_t *markobot;
extern TIM_HandleTypeDef htim2;
uint16_t left_sensor_data, right_sensor_data, front_data;

TEST_GROUP(MotorControllers)
{
  void setup()
  {
    markobot = robot_create();
    UT_PTR_SET(update_pwm_value, update_pwm_fake);
  }

  void teardown()
  {
    robot_destroy(markobot);
  }
};


TEST(MotorControllers, ErrorTermIsCalculatedForAppropriateValues)
{
  left_sensor_data = 30;
  right_sensor_data = 40;
  motor_controller_centring(&htim2, right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL((int16_t) 10 * CENTRING_P_GAIN, motor_controller_spy_GetLastError());
}

TEST(MotorControllers, ErrorCalculationRejectsOutOfRangeValues)
{
  left_sensor_data = 155;
  right_sensor_data = 30;
  motor_controller_centring(&htim2, right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL(0, motor_controller_spy_GetLastError());
  left_sensor_data = 11;
  right_sensor_data = 250;
  motor_controller_centring(&htim2, right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL(0, motor_controller_spy_GetLastError());
}


