#ifndef _FAKE_SENSORS_H
#define _FAKE_SENSORS_H
#include "sensors.h"
int sensor_read_stub(sensors_t sensor, uint16_t *data);
void sensor_inject_fake_data(sensors_t sensor, uint16_t lies);
#endif
