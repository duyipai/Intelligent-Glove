#include <BCDS_Basics.h>
#include <BCDS_Retcode.h>
#include <XdkSensorHandle.h>
#include <BCDS_Environmental.h>
#include "XdkExceptions.h"


Retcode_T SensorEnvironment_Setup(void)
{
	Retcode_T exception = RETCODE_OK;

	exception = Environmental_init(xdkEnvironmental_BME280_Handle);
	if(exception == NO_EXCEPTION)
	{
		exception = Accelerometer_init(xdkAccelerometers_BMA280_Handle);
	}
	if(exception == NO_EXCEPTION)
	{
		exception = Accelerometer_init(xdkAccelerometers_BMA280_Handle);
	}
	if(exception == NO_EXCEPTION)
	{
		exception = Gyroscope_init(xdkGyroscope_BMG160_Handle);
	}
	if(exception == NO_EXCEPTION)
	{
		exception = Accelerometer_init(xdkAccelerometers_BMI160_Handle);
	}
	if(exception == NO_EXCEPTION)
	{
		exception = Gyroscope_init(xdkGyroscope_BMI160_Handle);
	}
	if(exception == NO_EXCEPTION)
	{
		exception = Magnetometer_init(xdkMagnetometer_BMM150_Handle);
	}
	else
	{
		return exception;
	}
	return NO_EXCEPTION;
}

Retcode_T SensorEnvironment_Enable(void)
{

	return NO_EXCEPTION;
}


