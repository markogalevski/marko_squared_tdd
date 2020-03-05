#include "sensors.h"

VL53L0X_Dev_t left, front_left, front, front_right, right;
static VL53L0X_Dev_t *sensors[NUM_SENSORS] =
    {
	(VL53L0X_Dev_t *)&left,
	(VL53L0X_Dev_t *)&front_left,
	(VL53L0X_Dev_t *)&front,
	(VL53L0X_Dev_t *)&front_right,
	(VL53L0X_Dev_t *)&right,
    };

int sensor_init(sensors_t sensor, uint8_t slave_address,
		  uint32_t interrupt_threshold_mm, VL53L0X_GpioFunctionality interrupt_behaviour)
{
  // Initialize Comms
  VL53L0X_Dev_t *devicePointer = sensors[sensor];
  devicePointer->I2cDevAddr      = slave_address;
  devicePointer->comms_type      =  1;
  devicePointer->comms_speed_khz =  400;

  int8_t status = VL53L0X_DataInit(devicePointer);
  if (status) return(status);

  status = VL53L0X_StaticInit(devicePointer); // Device Initialization
  if (status) return(status);


  uint32_t refSpadCount;
  uint8_t isApertureSpads;
  uint8_t VhvSettings;
  uint8_t PhaseCal;

  status = VL53L0X_PerformRefCalibration(devicePointer,
      		&VhvSettings, &PhaseCal); // Device Initialization
  if (status) return(status);

  status = VL53L0X_PerformRefSpadManagement(devicePointer,
          		&refSpadCount, &isApertureSpads); // Device Initialization
  if (status) return(status);

  status = VL53L0X_SetDeviceMode(devicePointer, VL53L0X_DEVICEMODE_SINGLE_RANGING);
  if (status) return(status);

  status = VL53L0X_SetLimitCheckEnable(devicePointer,
          		VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
  if (status) return(status);

  status = VL53L0X_SetLimitCheckEnable(devicePointer,
          		VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
  if (status) return(status);

  status = VL53L0X_SetLimitCheckEnable(devicePointer,
          		VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
  if (status) return(status);

  status = VL53L0X_SetLimitCheckValue(devicePointer,
          		VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
          		(FixPoint1616_t)(1.5*0.023*65536));
  if (status) return(status);

  status = VL53L0X_SetGpioConfig(devicePointer, 0, VL53L0X_DEVICEMODE_SINGLE_RANGING,
			  interrupt_behaviour, VL53L0X_INTERRUPTPOLARITY_HIGH);
  if (status) return(status);

  status = VL53L0X_SetInterruptThresholds(devicePointer,VL53L0X_DEVICEMODE_SINGLE_RANGING,
				 interrupt_threshold_mm, interrupt_threshold_mm);
  if(status) return(status);

  status = VL53L0X_EnableInterruptMask(devicePointer, 1);

  return(status);

}


int sensor_read_impl(sensors_t sensor, uint16_t *data)
{

  VL53L0X_Dev_t *devicePointer = sensors[sensor];
  VL53L0X_RangingMeasurementData_t    RangingMeasurementData;
  FixPoint1616_t LimitCheckCurrent;
  int8_t status = VL53L0X_PerformSingleRangingMeasurement(devicePointer,
  		&RangingMeasurementData);
  if (status) return(status);

  status = VL53L0X_GetLimitCheckCurrent(devicePointer,
              		VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, &LimitCheckCurrent);
  if(status) return(status);

  *data = RangingMeasurementData.RangeMilliMeter;
  return(0);
}
int (*sensor_read)(sensors_t sensor, uint16_t *data) = &sensor_read_impl;
