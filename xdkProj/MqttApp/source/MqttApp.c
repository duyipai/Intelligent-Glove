#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <XdkSensorHandle.h>
#include <BCDS_Environmental.h>
#include <stdbool.h>
#include <BCDS_Basics.h>
#include <BCDS_Retcode.h>
#include "ConnectivityMQTTMessaging.h"
#include "SensorEnvironment.h"
#include "XdkExceptions.h"
#include "MqttApp.h"

/*
Retcode_T HandleEvery10mS(void)
{
	Retcode_T exception = NO_EXCEPTION;
	char result[60];
	char* _newConnectivityMQTTMessaging_Test_0;

	Environmental_Data_T myEnvironment;
	Accelerometer_XyzData_T myAccelEmbedded, myAccelInertial;
	Gyroscope_XyzData_T myConvEmbedded, myConvInertial;
	Magnetometer_XyzData_T myMag;

	while(1){
		Environmental_readData(xdkEnvironmental_BME280_Handle, &myEnvironment);
		Accelerometer_readXyzGValue(xdkAccelerometers_BMA280_Handle, &myAccelEmbedded);
		Accelerometer_readXyzGValue(xdkAccelerometers_BMI160_Handle, &myAccelInertial);
		Gyroscope_readXyzDegreeValue(xdkGyroscope_BMG160_Handle, &myConvEmbedded);
		Gyroscope_readXyzDegreeValue(xdkGyroscope_BMI160_Handle, &myConvInertial);
		Magnetometer_readXyzTeslaData(xdkMagnetometer_BMM150_Handle, &myMag);


		printf(	"%ld %ld %ld "
				"%ld %ld %ld "
				"%ld %ld %ld "
				"published\n",
				(long int) (6923*myAccelEmbedded.xAxisData + 3077*myAccelInertial.xAxisData)/10000,
				(long int) (6923*myAccelEmbedded.yAxisData + 3077*myAccelInertial.yAxisData)/10000,
				(long int) (6923*myAccelEmbedded.zAxisData + 3077*myAccelInertial.zAxisData)/10000,
				(long int) (2462*myConvEmbedded.xAxisData + 7538*myConvInertial.xAxisData)/10000,
				(long int) (2462*myConvEmbedded.yAxisData + 7538*myConvInertial.yAxisData)/10000,
				(long int) (2462*myConvEmbedded.zAxisData + 7538*myConvInertial.zAxisData)/10000,
				(long int) myMag.xAxisData,(long int) myMag.yAxisData, (long int) myMag.zAxisData);

		snprintf(result, sizeof(result),
				"%0d %0d %0d "
				"%0d %0d %0d "
				"%0d %0d %0d "
				"\n",
				(6923*myAccelEmbedded.xAxisData + 3077*myAccelInertial.xAxisData)/10000,
				(6923*myAccelEmbedded.yAxisData + 3077*myAccelInertial.yAxisData)/10000,
				(6923*myAccelEmbedded.zAxisData + 3077*myAccelInertial.zAxisData)/10000,
				(2462*myConvEmbedded.xAxisData + 7538*myConvInertial.xAxisData)/10000,
				(2462*myConvEmbedded.yAxisData + 7538*myConvInertial.yAxisData)/10000,
				(2462*myConvEmbedded.zAxisData + 7538*myConvInertial.zAxisData)/10000,
				myMag.xAxisData, myMag.yAxisData, myMag.zAxisData);

		_newConnectivityMQTTMessaging_Test_0 = result;
		exception = ConnectivityMQTTMessaging_Test_Write(&_newConnectivityMQTTMessaging_Test_0);
	}
		if(exception != NO_EXCEPTION) return exception;

	return NO_EXCEPTION;
}
*/
Retcode_T HandleEvery10mS(void* userParameter1, uint32_t userParameter2)
{

	BCDS_UNUSED(userParameter1);
	BCDS_UNUSED(userParameter2);

	Retcode_T exception = NO_EXCEPTION;
	char result[60];

	Environmental_Data_T myEnvironment;
	Accelerometer_XyzData_T myAccelEmbedded, myAccelInertial;
	Gyroscope_XyzData_T myConvInertial;
	Magnetometer_XyzData_T myMag;

	exception = Environmental_readData(xdkEnvironmental_BME280_Handle, &myEnvironment);
	exception = Accelerometer_readXyzGValue(xdkAccelerometers_BMA280_Handle, &myAccelEmbedded);
	exception = Accelerometer_readXyzGValue(xdkAccelerometers_BMI160_Handle, &myAccelInertial);
	//exception = Gyroscope_readXyzDegreeValue(xdkGyroscope_BMG160_Handle, &myConvEmbedded);
	exception = Gyroscope_readXyzDegreeValue(xdkGyroscope_BMI160_Handle, &myConvInertial);
	exception = Magnetometer_readXyzTeslaData(xdkMagnetometer_BMM150_Handle, &myMag);
	if(exception != NO_EXCEPTION) return exception;

	/*
	printf(	"%ld %ld %ld "
			"%ld %ld %ld "
			"%ld %ld %ld "
			"published\n",
			(long int) (6923*myAccelEmbedded.xAxisData + 3077*myAccelInertial.xAxisData)/10000,
			(long int) (6923*myAccelEmbedded.yAxisData + 3077*myAccelInertial.yAxisData)/10000,
			(long int) (6923*myAccelEmbedded.zAxisData + 3077*myAccelInertial.zAxisData)/10000,
			(long int) (2462*myConvEmbedded.xAxisData + 7538*myConvInertial.xAxisData)/10000,
			(long int) (2462*myConvEmbedded.yAxisData + 7538*myConvInertial.yAxisData)/10000,
			(long int) (2462*myConvEmbedded.zAxisData + 7538*myConvInertial.zAxisData)/10000,
			(long int) myMag.xAxisData,(long int) myMag.yAxisData, (long int) myMag.zAxisData);
	*/
	snprintf(result, sizeof(result),
			"%d "
			"%d %d %d "
			"%d %d %d "
			"%d %d %d "
			"\n",
			(int) xTaskGetTickCountFromISR(),
			(6923*myAccelEmbedded.xAxisData + 3077*myAccelInertial.xAxisData)/10000,
			(6923*myAccelEmbedded.yAxisData + 3077*myAccelInertial.yAxisData)/10000,
			(6923*myAccelEmbedded.zAxisData + 3077*myAccelInertial.zAxisData)/10000,
			myConvInertial.xAxisData,
			myConvInertial.yAxisData,
			myConvInertial.zAxisData,
			myMag.xAxisData, myMag.yAxisData, myMag.zAxisData);

	char* _newConnectivityMQTTMessaging_Test_0 = result;
	exception = ConnectivityMQTTMessaging_Test_Write(&_newConnectivityMQTTMessaging_Test_0);
	if(exception != NO_EXCEPTION) return exception;


	return NO_EXCEPTION;
}

Retcode_T HandleEveryButtonOnePressed(void* userParameter1, uint32_t userParameter2)
{

	BCDS_UNUSED(userParameter1);
	BCDS_UNUSED(userParameter2);

	Retcode_T exception = NO_EXCEPTION;


	char* _newConnectivityMQTTMessaging_Test_0 = "button pressed\n";
	exception = ConnectivityMQTTMessaging_Test_Write(&_newConnectivityMQTTMessaging_Test_0);
	if(exception != NO_EXCEPTION) return exception;


	return NO_EXCEPTION;
}
