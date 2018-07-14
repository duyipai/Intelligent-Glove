/*
* Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licensee�s application development. 
* Fitness and suitability of the example code for any use within Licensee�s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
* Licensee shall be responsible for conducting the development of its applications as well as integration of parts of the example code into such applications, taking into account the state of the art of technology and any statutory regulations and provisions applicable for such applications. Compliance with the functional system requirements and testing there of (including validation of information/data security aspects and functional safety) and release shall be solely incumbent upon Licensee. 
* For the avoidance of doubt, Licensee shall be responsible and fully liable for the applications and any distribution of such applications into the market.
* 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are 
* met:
* 
*     (1) Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer. 
* 
*     (2) Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.  
*     
*     (3)The name of the author may not be used to
*     endorse or promote products derived from this software without
*     specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*/
/*----------------------------------------------------------------------------*/
/**
* @ingroup STREAM_SENSOR_DATA_OVER_USB
*
* @defgroup INERTIAL_SENSOR InertialSensor
* @{
*
* @brief BME280 Environmental data on serialport
*
* @details Demo application of printing BMI160 InertialSensor data on serialport(USB virtual comport)
*          every one second, initiated by InertialSensor timer(freertos).
*
* @file
**/

//lint -esym(956,*) /* Suppressing "Non const, non volatile static or external variable" lint warning*/
/* module includes ********************************************************** */

/* own header files */
#include "XDKAppInfo.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID XDK_APP_MODULE_ID_INERTIAL_SENSOR
#include "InertialSensor.h"
#include "XdkSensorHandle.h"

/* additional interface header files */
#include "BCDS_Basics.h"
#include "BCDS_Accelerometer.h"
#include "BCDS_Gyroscope.h"
#include "BCDS_Assert.h"
#include "BCDS_CmdProcessor.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "StreamSensorDataOverUsb.h"

/* system header files */
#include <stdio.h>

/* constant definitions ***************************************************** */
#define TIMER_NOT_ENOUGH_MEMORY            (-1L)/**<Macro to define not enough memory error in timer*/
#define TIMER_AUTORELOAD_ON             UINT32_C(1)             /**< Auto reload of timer is enabled*/
#define THREESECONDDELAY  UINT32_C(10)      /**< three seconds delay is represented by this macro */
#define TIMERBLOCKTIME  UINT32_C(0xffff)    /** Macro used to define blocktime of a timer*/

/* local variables ********************************************************** */
static xTimerHandle printTimerHandle = 0;/** variable to store timer handle*/

/* global variables ********************************************************* */

/** The function is to print thes inertial sensor  data  from  inertial sensor  on serialport.
 * @brief Gets the raw data from inertial sensor  and print inertial sensor sensor data over USB.
 *
 * @param[in] param1 should be defined with the type void *(as argument)
 *
 *
 * @param[in] param2 should be defined with the type uint32_t (as argument)
 */

static void processInertiaSensor(void * param1, uint32_t param2)
{
    BCDS_UNUSED(param1);
    BCDS_UNUSED(param2);

    /* Return value for Accel Sensor */
    Retcode_T accelReturnValue = (Retcode_T) RETCODE_FAILURE;
    Retcode_T gyroReturnValue = (Retcode_T) RETCODE_FAILURE;
    Accelerometer_XyzData_T getaccelData = { INT32_C(0), INT32_C(0), INT32_C(0) };
    Gyroscope_XyzData_T getRawData = { INT32_C(0), INT32_C(0), INT32_C(0) };
    Gyroscope_XyzData_T getConvData = { INT32_C(0), INT32_C(0), INT32_C(0) };
    accelReturnValue = Accelerometer_readXyzLsbValue(xdkAccelerometers_BMI160_Handle, &getaccelData);
    if (RETCODE_OK == accelReturnValue)
    {
        /*print Accel data of BMI160 on serialport */
        //printf("BMI160 Accel Raw Data :\n\rx =%ld\n\ry =%ld\n\rz =%ld\n\r",
          //      (long int) getaccelData.xAxisData, (long int) getaccelData.yAxisData, (long int) getaccelData.zAxisData);
    }
    else
    {
        printf("BMI160 Read Raw Data Failed\n\r");
    }
    accelReturnValue = Accelerometer_readXyzGValue(xdkAccelerometers_BMI160_Handle, &getaccelData);
    if (RETCODE_OK == accelReturnValue)
    {
        /*print  Accel data of BMI160 on serialport */
        printf("Acc: %ld %ld %ld \n",
                (long int) getaccelData.xAxisData, (long int) getaccelData.yAxisData, (long int) getaccelData.zAxisData);
    }
    else
    {
        printf("BMI160 Read G Data Failed\n\r");
    }
    gyroReturnValue = Gyroscope_readXyzValue(xdkGyroscope_BMI160_Handle, &getRawData);
    if (RETCODE_OK == gyroReturnValue)
    {
        //printf("BMI160 Gyro Raw Data :\n\rx =%ld\n\ry =%ld\n\rz =%ld\n\r",
         //       (long int) getRawData.xAxisData, (long int) getRawData.yAxisData, (long int) getRawData.zAxisData);
    }
    else
    {
        printf("BMI160 GyrosensorReadRawData Failed\n\r");
    }
    /* read sensor data in milli Degree*/
    gyroReturnValue = Gyroscope_readXyzDegreeValue(xdkGyroscope_BMI160_Handle, &getConvData);
    if (RETCODE_OK == gyroReturnValue)
    {
        printf("Gyr: %ld %ld %ld \n",
                (long int) getConvData.xAxisData, (long int) getConvData.yAxisData, (long int) getConvData.zAxisData);

    }
    else
    {
        printf("BMI160 GyrosensorReadInMilliDeg Failed\n\r");
    }
    //printf("---------------------------------------------------------\n\r");
}

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/** The function to get and print the accel data using printf
 * @brief Gets the data from BMI160 Accel and prints through the USB printf on serial port
 *
 * @param[in] pvParameters Rtos task should be defined with the type void *(as argument)
 */
static void printInertialData(void *pvParameters)
{
    BCDS_UNUSED(pvParameters);
    Retcode_T returnValue = RETCODE_OK;
    returnValue = CmdProcessor_Enqueue(GetAppCmdProcessorHandle(), processInertiaSensor, NULL, UINT32_C(0));
    if (RETCODE_OK != returnValue)
    {
        printf("Enqueuing for inertial sensor callback failed\n\r");
    }
}

/* global functions ********************************************************* */

Retcode_T inertialSensorInit(void)
{
    /* Return value for Timer start */
    int8_t retValPerSwTimer = TIMER_NOT_ENOUGH_MEMORY;

    /* Return value for Accel Sensor */
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;

    /*initialize accel*/
    returnValue = Accelerometer_init(xdkAccelerometers_BMI160_Handle);
    if (RETCODE_OK == returnValue)
    {
        returnValue = Gyroscope_init(xdkGyroscope_BMI160_Handle);
    }
    if ((RETCODE_OK == returnValue))
    {
        uint32_t Ticks = THREESECONDDELAY;

        if (Ticks != UINT32_MAX) /* Validated for portMAX_DELAY to assist the task to wait Infinitely (without timing out) */
        {
            Ticks /= portTICK_RATE_MS;
        }
        if (UINT32_C(0) == Ticks) /* ticks cannot be 0 in FreeRTOS timer. So ticks is assigned to 1 */
        {
            Ticks = UINT32_C(1);
        }
        /* create timer task to get and print accel/Gyro data every three second automatically*/
        printTimerHandle = xTimerCreate((const char * const ) "printInertialData",
                Ticks, TIMER_AUTORELOAD_ON, NULL, printInertialData);

        /* PID timer create fail case */
        if (NULL == printTimerHandle)
        {
            /* Assertion Reason : "This software timer was not Created, Due to Insufficient heap memory" */
            assert(false);
        }

        /*start the created timer*/
        retValPerSwTimer = xTimerStart(printTimerHandle,
                TIMERBLOCKTIME);

        /* PID timer start fail case */
        if (TIMER_NOT_ENOUGH_MEMORY == retValPerSwTimer)
        {
            /* Assertion Reason : "This software timer was not started, Due to Insufficient heap memory" */
            assert(false);
        }
    }
    else
    {
        printf("InertialSensor init Failed\n\r");
    }
    return returnValue;
}

Retcode_T inertialSensorDeinit(void)
{

    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;

    /*initialize accel*/
    returnValue = Accelerometer_deInit(xdkAccelerometers_BMI160_Handle);
    if (RETCODE_OK == returnValue)
    {
        /*initialize gyro*/
        returnValue = Gyroscope_deInit(xdkGyroscope_BMI160_Handle);
    }

    if (RETCODE_OK == returnValue)
    {
        printf("InertialSensor Deinit Success\n\r");
    }
    else
    {
        printf("InertialSensor Deinit Failed\n\r");
    }
    return returnValue;
}
/**@} */
/** ************************************************************************* */
