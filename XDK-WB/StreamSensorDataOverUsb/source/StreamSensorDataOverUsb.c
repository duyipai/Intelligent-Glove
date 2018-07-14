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
* @ingroup APPS_LIST
*
* @defgroup STREAM_SENSOR_DATA_OVER_USB Stream Sensor Data Over Usb
* @{
*
* @brief Application of printing all the defined sensors on serialport
*
* @details Demo application of printing all the defined sensors on serialport(USB virtual comport)
*          every one second, initiated by autoreloaded timer(freertos)
*
* @file
**/

/* module includes ********************************************************** */

/* own header files */
#include "XDKAppInfo.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID XDK_APP_MODULE_ID_SEND_STREAM_SENSOR
#include "Gyroscope.h"
#include "LightSensor.h"
#include "InertialSensor.h"
#include "Magnetometer.h"
#include "EnvironmentalSensor.h"
#include "Accelerometer.h"

/* additional interface header files */
#include "BCDS_Basics.h"
#include "BCDS_Assert.h"
#include "BCDS_TaskConfig.h"
#include "BCDS_CmdProcessor.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "StreamSensorDataOverUsb.h"

/* system header files */
#include <stdio.h>

CmdProcessor_T *AppCmdProcessorHandle;

/*Application Command Processor Instance */

void appInitSystem(void * CmdProcessorHandle, uint32_t param2)
{
    if (CmdProcessorHandle == NULL)
    {
        printf("Command processor handle is null \n\r");
        assert(false);
    }
    AppCmdProcessorHandle = (CmdProcessor_T *) CmdProcessorHandle;
    BCDS_UNUSED(param2);
    Retcode_T returnValue = RETCODE_OK;

    /* Initialize all sensors */
    returnValue = accelerometerSensorInit();
    if (RETCODE_OK == returnValue)
    {
       // returnValue = environmentalSensorInit();
    }
    if (RETCODE_OK == returnValue)
    {
        returnValue = gyroscopeSensorInit();
    }
    if (RETCODE_OK == returnValue)
    {
        returnValue = inertialSensorInit();
    }
    if (RETCODE_OK == returnValue)
    {
        returnValue = magnetometerSensorInit();
    }
    if (RETCODE_OK == returnValue)
    {
        //returnValue = lightsensorInit();
    }
    if (RETCODE_OK == returnValue)
    {
        printf("Sensors intialization succeeded");
    }
    else
    {
        printf("Sensors intialization failed");
    }

}


Retcode_T deinit(void)
{
    Retcode_T returnValue = RETCODE_OK;
    returnValue = accelerometerSensorDeinit();

    if (RETCODE_OK == returnValue)
    {
        //returnValue = environmentalSensorDeinit();
    }
    if (RETCODE_OK == returnValue)
    {
        returnValue = gyroscopeSensorDeinit();
    }
    if (RETCODE_OK == returnValue)
    {
        returnValue = inertialSensorDeinit();
    }
    return returnValue;
}

CmdProcessor_T * GetAppCmdProcessorHandle(void)
{
    return AppCmdProcessorHandle;
}
/**@} */
/** ************************************************************************* */
