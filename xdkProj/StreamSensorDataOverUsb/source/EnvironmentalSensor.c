/*
* Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licensee’s application development. 
* Fitness and suitability of the example code for any use within Licensee’s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
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
* @defgroup ENVIRONMENTAL_SENSOR EnvironmentalSensor
* @{
*
* @brief BME280 Environmental data on serialport
*
* @details Demo application of printing BME280 Environmental data on serialport(USB virtual comport)
 *          every one second, initiated by Environmental timer(freertos).
*
* @file
**/

//lint -esym(956,*) /* Suppressing "Non const, non volatile static or external variable" lint warning*/
/* module includes ********************************************************** */

/* own header files */
#include "XDKAppInfo.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID XDK_APP_MODULE_ID_ENVIRONMENTAL_SENSOR
#include "EnvironmentalSensor.h"
#include "XdkSensorHandle.h"

/* additional interface header files */
#include "BCDS_Basics.h"
#include "BCDS_Environmental.h"
#include "BCDS_Assert.h"
#include "BCDS_CmdProcessor.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "StreamSensorDataOverUsb.h"

/* system header files */
#include <stdio.h>

/* constant definitions ***************************************************** */
#define THREESECONDDELAY  UINT32_C(3000)        /**< three seconds delay is represented by this macro */
#define TIMERBLOCKTIME  UINT32_C(0xffff)    /** Macro used to define blocktime of a timer*/
#define TIMER_NOT_ENOUGH_MEMORY            (-1L)/**<Macro to define not enough memory error in timer*/
#define TIMER_AUTORELOAD_ON             UINT32_C(1)             /**< Auto reload of timer is enabled*/

/* local variables ********************************************************** */
static xTimerHandle printTimerHandle;/**< variable to store timer handle*/
/* global variables ********************************************************* */

/** The function is to print the Environmental data  from  BME280 Environmental on serialport.
 * @brief Gets the raw data from BME280 Environmental  and print BME280 Environmental sensor data over USB.
 *
 * @param[in] param1 should be defined with the type void *(as argument)
 *
 *
 * @param[in] param2 should be defined with the type uint32_t (as argument)
 */

static void processEnvSensorData(void * param1, uint32_t param2)
{
    BCDS_UNUSED(param1);
    BCDS_UNUSED(param2);

    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;
    Environmental_Data_T bme280 = { INT32_C(0), UINT32_C(0), UINT32_C(0) };
    Environmental_LsbData_T bme280lsb = { INT32_C(0), INT32_C(0), INT32_C(0) };
    returnValue = Environmental_readDataLSB(xdkEnvironmental_BME280_Handle, &bme280lsb);
    if ( RETCODE_OK == returnValue)
    {
        /*print chip id and Environmental data of BME280 on serialport */
        printf("BME280 Environmental Raw Data :\n\rp =%ld\n\rt =%ld\n\rh =%ld\n\r",
                (long int) bme280lsb.pressure, (long int) bme280lsb.temperature, (long int) bme280lsb.humidity);
    }
    else
    {
        printf("Environmental Read Raw Data Failed\n\r");
    }
    /* Read temperature,pressure,humidity actual values */
    returnValue = Environmental_readData(xdkEnvironmental_BME280_Handle, &bme280);
    if ( RETCODE_OK == returnValue)
    {
        /*print chip id and Environmental data of BME280 on serialport */
        printf("BME280 Environmental Conversion Data :\n\rp =%ld Pa\n\rt =%ld mDeg\n\rh =%ld %%rh\n\r",
                (long int) bme280.pressure, (long int) bme280.temperature, (long int) bme280.humidity);
    }
    else
    {
        printf("Environmental Read actual Data Failed\n\r");
    }
}

/* inline functions ********************************************************* */

/* local functions ********************************************************** */
/** The function to get and print the Environmental data using printf
 * @brief Gets the raw data from BME280 Environmental and prints through the USB printf on serial port
 *
 * @param[in] pvParameters Rtos task should be defined with the type void *(as argument)
 */
static void printEnvData(void *pvParameters)
{
    BCDS_UNUSED(pvParameters);

    Retcode_T returnValue = RETCODE_OK;
    returnValue = CmdProcessor_Enqueue(GetAppCmdProcessorHandle(), processEnvSensorData, NULL, UINT32_C(0));
    if (RETCODE_OK != returnValue)
    {
        printf("Enqueuing for environmental sensor callback failed\n\r");
    }
}

/* global functions ********************************************************* */

Retcode_T environmentalSensorInit(void)
{
    /* Return value for Timer start */
    int8_t retValPerSwTimer = TIMER_NOT_ENOUGH_MEMORY;
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;

    /*initialize Environmental sensor*/
    returnValue = Environmental_init(xdkEnvironmental_BME280_Handle);
    if ( RETCODE_OK == returnValue)
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
        /* create timer task to get and print Environmental data every one second automatically*/
        printTimerHandle = xTimerCreate((const char * const ) "printEnvData",
                Ticks, TIMER_AUTORELOAD_ON, NULL, printEnvData);

        /* timer create fail case */
        if (NULL == printTimerHandle)
        {
            /* Assertion Reason : "This software timer was not Created, Due to Insufficient heap memory" */
            assert(false);
        }
        /*start the created timer*/
        retValPerSwTimer = xTimerStart(printTimerHandle,
                TIMERBLOCKTIME);

        /* PED timer start fail case */
        if (TIMER_NOT_ENOUGH_MEMORY == retValPerSwTimer)
        {
            /* Assertion Reason :  "This software timer was not started, Due to Insufficient heap memory" */
            assert(false);
        }
        printf("Environmental Sensor initialization Success\n\r");
    }
    else
    {
        printf("Environmental Sensor initialization Failed\n\r");
    }
    return returnValue;
}

Retcode_T environmentalSensorDeinit(void)
{
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;

    returnValue = Environmental_deInit(xdkEnvironmental_BME280_Handle);
    if (RETCODE_OK == returnValue)
    {
        printf("Environmental sensor Deinit Success\n\r");
    }
    else
    {
        printf("Environmental sensor Deinit Failed\n\r");
    }
    return returnValue;
}

/**@} */
/** ************************************************************************* */
