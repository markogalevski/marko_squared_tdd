
extern "C"
{
#include "pinout.h"
#include "robot.h"
#include "sensors_fake.h"
#include "main.h"
#include "mapping.h"
#include "motor_controller_spy.h"
}
#include "stm32f4xx_it.h"

#include "CppUTest/TestHarness.h"

extern robot_t *markobot;
extern TIM_HandleTypeDef htim2, htim3, htim4, htim5;
uint16_t left_sensor_data, right_sensor_data, front_data;

void MX_TIM2_Init(void);


TEST_GROUP(MotorControllerForward)
{
  void setup()
  {
    markobot = robot_create();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_GPIO_Init();
    UT_PTR_SET(update_pwm_value, update_pwm_fake);

  }

  void teardown()
  {
    robot_destroy(markobot);
  }
};


TEST(MotorControllerForward, RightValueLargerThanLeft)
{
  left_sensor_data = 30;
  right_sensor_data = 40;
  motor_controller_forward(right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL((int16_t) 10 * CENTRING_P_GAIN, motor_controller_spy_GetLastError());
}

TEST(MotorControllerForward, RightValueSmallerThanLeft)
{
  left_sensor_data = 40;
  right_sensor_data = 30;
  motor_controller_forward(right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL((int16_t) -10 * CENTRING_P_GAIN, motor_controller_spy_GetLastError());

}

TEST(MotorControllerForward, ErrorCalculationRejectsOutOfRangeValues)
{
  left_sensor_data = 155;
  right_sensor_data = 30;
  motor_controller_forward(right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL(0, motor_controller_spy_GetLastError());
  left_sensor_data = 11;
  right_sensor_data = 250;
  motor_controller_forward(right_sensor_data, left_sensor_data, CENTRING_P_GAIN);
  LONGS_EQUAL(0, motor_controller_spy_GetLastError());
}

TEST_GROUP(MotorControllerInPlace)
{
  void setup()
  {
    markobot = robot_create();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_GPIO_Init();
    UT_PTR_SET(motor_read_encoder, motor_read_encoder_fake);
    UT_PTR_SET(mapping_create_node, mapping_create_node_fake);


  }

  void teardown()
  {
    robot_destroy(markobot);
  }

};

TEST(MotorControllerInPlace, EnableFunctionWorks)
{
  motor_enable_all();
  LONGS_EQUAL(1, htim2.Instance->CCER & (0x01));
  LONGS_EQUAL(1, htim5.Instance->CCER & (0x01));
}

TEST(MotorControllerInPlace, DisableFunctionWorks)
{
  motor_enable_all();
  motor_disable_all();
  LONGS_EQUAL(0, htim2.Instance->CCER & (0x01));
  LONGS_EQUAL(0, htim5.Instance->CCER & (0x01));
}

TEST(MotorControllerInPlace, TurnsLeft)
{
  uint32_t start_encoder = motor_read_encoder(ENCODER_R);
  motor_controller_in_place(LEFT_90_DEG);
  uint32_t after_encoder = motor_read_encoder(ENCODER_R) - 2; //minus 2 to account for the increment of the fake impl
  LONGS_EQUAL(ENCODER_90_TURN, after_encoder - start_encoder);
  LONGS_EQUAL(0, htim2.Instance->CCER & (0x01));
  LONGS_EQUAL(0, htim5.Instance->CCER & (0x01));
}

TEST(MotorControllerInPlace, TurnsRight)
{
  uint32_t start_encoder = motor_read_encoder(ENCODER_L);
  motor_controller_in_place(RIGHT_90_DEG);
  uint32_t after_encoder = motor_read_encoder(ENCODER_L) - 2;
  LONGS_EQUAL(ENCODER_90_TURN, after_encoder - start_encoder);
  LONGS_EQUAL(0, htim2.Instance->CCER & (0x01));
  LONGS_EQUAL(0, htim5.Instance->CCER & (0x01));
}

TEST(MotorControllerInPlace, TurnsAround)
{
  uint32_t start_encoder = motor_read_encoder(ENCODER_R);
  motor_controller_in_place(LEFT_180_DEG);
  uint32_t after_encoder = motor_read_encoder(ENCODER_R) - 2;
  LONGS_EQUAL(ENCODER_180_TURN, after_encoder - start_encoder);
  LONGS_EQUAL(0, htim2.Instance->CCER & (0x01));
  LONGS_EQUAL(0, htim5.Instance->CCER & (0x01));
}

TEST_GROUP(MotorControllerCoast)
{
  void setup()
  {

    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_GPIO_Init();
    UT_PTR_SET(motor_read_encoder, motor_read_encoder_fake);
    UT_PTR_SET(mapping_create_node, mapping_create_node_fake);

  }

  void teardown()
  {
  }
};

TEST(MotorControllerCoast, ProperlyCoasts)
{
  uint32_t start_encoder = motor_read_encoder(ENCODER_R);
  motor_controller_coast(7.0);
  uint32_t end_encoder = motor_read_encoder(ENCODER_R) - 3;
  LONGS_EQUAL(CM_TO_TICKS(7.0), end_encoder - start_encoder);
}

