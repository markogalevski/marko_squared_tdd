#include "../Inc/sensors_fake.h"


static uint16_t fake_data[NUM_SENSORS];

int sensor_read_stub(sensors_t sensor, uint16_t *data)
{
  *data = fake_data[sensor];
  return(0);
}

void sensor_inject_fake_data(sensors_t sensor, uint16_t lies)
{
  fake_data[sensor] = lies;
}
