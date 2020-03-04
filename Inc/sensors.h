#ifndef _SENSORS_H
#define _SENSORS_H



#include "stm32f4xx_hal.h"
#include "vl53l0x_api.h"
#include <string.h>
#include <limits.h>

int sensor_init(VL53L0X_Dev_t *devicePointer, uint8_t slave_address,
		  uint32_t interrupt_threshold_mm, VL53L0X_GpioFunctionality interrupt_behaviour);
int sensor_read(VL53L0X_Dev_t *devicePointer, uint16_t *data);
#endif
