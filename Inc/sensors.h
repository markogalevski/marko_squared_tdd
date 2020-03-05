#ifndef _SENSORS_H
#define _SENSORS_H

#include "stm32f4xx_hal.h"
#include "vl53l0x_api.h"
#include <string.h>
#include <limits.h>

#define LEFT_SENSOR_ADDRESS 	0x32
#define RIGHT_SENSOR_ADDRESS	0x33
#define FRONT_SENSOR_ADDRESS	0x34

typedef enum
{
  LEFT_SENSOR,
  FRONT_LEFT_SENSOR,
  FRONT_SENSOR,
  FRONT_RIGHT_SENSOR,
  RIGHT_SENSOR,
  NUM_SENSORS
}sensors_t;

int sensor_init(sensors_t sensor,  uint8_t slave_address,
		  uint32_t interrupt_threshold_mm, VL53L0X_GpioFunctionality interrupt_behaviour);
extern int (*sensor_read)(sensors_t sensor, uint16_t *data);
#endif
