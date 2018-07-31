#include <BCDS_Basics.h>
#include <FreeRTOS.h>
#include <inttypes.h>
#include <BCDS_Retcode.h>
#include <stdio.h>
#include <BCDS_CmdProcessor.h>
#include <XdkSystemStartup.h>
#include <timers.h>
#include "SensorEnvironment.h"
#include "ConnectivityMQTTMessaging.h"
#include "ConnectivityWLANWireless.h"
#include "XdkTime.h"
#include "XdkEvents.h"
#include "XdkExceptions.h"

Retcode_T XdkLive_initialize(void);
Retcode_T XdkLive_goLive(void* userParameter1, uint32_t userParameter2);
void InternalHandle(TimerHandle_t xTimer);

CmdProcessor_T XdkLive_EventQueue;
TimerHandle_t timerHandle;

#define TASK_PRIO_EVENT_LOOP         (UINT32_C(1))
#define TASK_STACK_SIZE_EVENT_LOOP   (UINT16_C(700))
#define TASK_Q_LEN_EVENT_LOOP        (UINT32_C(10))

int main(void)
{
	/* Mapping Default Error Handling function */
	Retcode_T returnValue = Retcode_initialize(DefaultErrorHandlingFunc);
	if (RETCODE_OK == returnValue)
	{
	    returnValue = systemStartup();
	}
	if (RETCODE_OK == returnValue)
	{
		returnValue = CmdProcessor_Initialize(&XdkLive_EventQueue, (char *) "EventQueue", TASK_PRIO_EVENT_LOOP, TASK_STACK_SIZE_EVENT_LOOP, TASK_Q_LEN_EVENT_LOOP);
	}
	if (RETCODE_OK == returnValue)
	{
		returnValue = CmdProcessor_Enqueue(&XdkLive_EventQueue, XdkLive_initialize, NULL, UINT32_C(0));
	}
	if (RETCODE_OK == returnValue)
	{
	    returnValue = CmdProcessor_Enqueue(&XdkLive_EventQueue, XdkLive_goLive, NULL, UINT32_C(0));
	}
	if (RETCODE_OK != returnValue)
	{
	    printf("System Startup failed");
	    assert(false);
	}
	/* start scheduler */
	vTaskStartScheduler();
}

Retcode_T XdkLive_initialize(void)
{
	Retcode_T exception = NO_EXCEPTION;


	// setup time
	exception = SetupTime();
	if(exception == NO_EXCEPTION)
	{
		printf("setup Time succeeded\n");
	}
	else
	{
		printf("failed to setup Time\n");
		return exception;
	}

	// setup resources
	exception = ConnectivityWLANWireless_Setup();
	if(exception == NO_EXCEPTION)
	{
		printf("setup ConnectivityWLANWireless succeeded\n");
	}
	else
	{
		printf("failed to setup ConnectivityWLANWireless\n");
		return exception;
	}

	exception = ConnectivityMQTTMessaging_Setup();
	if(exception == NO_EXCEPTION)
	{
		printf("setup ConnectivityMQTTMessaging succeeded\n");
	}
	else
	{
		printf("failed to setup ConnectivityMQTTMessaging\n");
		return exception;
	}

	exception = SensorEnvironment_Setup();
	if(exception == NO_EXCEPTION)
	{
		printf("setup SensorEnvironment succeeded\n");
	}
	else
	{
		printf("failed to setup SensorEnvironment\n");
		return exception;
	}


	return NO_EXCEPTION;
}

Retcode_T XdkLive_goLive(void* userParameter1, uint32_t userParameter2)
{
	BCDS_UNUSED(userParameter1);
	BCDS_UNUSED(userParameter2);
	Retcode_T exception = NO_EXCEPTION;

	exception = ConnectivityWLANWireless_Enable();
	if(exception == NO_EXCEPTION)
	{
		printf("[INFO, %s:%d] enable ConnectivityWLANWireless succeeded\n", __FILE__, __LINE__);
	}
	else
	{
		printf("[ERROR, %s:%d] failed to enable ConnectivityWLANWireless\n", __FILE__, __LINE__);
		return exception;
	}
	exception = ConnectivityMQTTMessaging_Enable();
	if(exception == NO_EXCEPTION)
	{
		printf("[INFO, %s:%d] enable ConnectivityMQTTMessaging succeeded\n", __FILE__, __LINE__);
	}
	else
	{
		printf("[ERROR, %s:%d] failed to enable ConnectivityMQTTMessaging\n", __FILE__, __LINE__);
		return exception;
	}
	timerHandle = xTimerCreate("timerHandle", UINT32_C(200), pdTRUE, NULL, InternalHandle);
	xTimerStart(timerHandle, UINT32_C(0xffff));
	if(exception == NO_EXCEPTION)
	{
		printf("[INFO, %s:%d] enable SensorEnvironment succeeded\n", __FILE__, __LINE__);
	}
	else
	{
		printf("[ERROR, %s:%d] failed to enable SensorEnvironment\n", __FILE__, __LINE__);
		return exception;
	}

	return NO_EXCEPTION;
}

void InternalHandle(void *pvParameters)
{
	BCDS_UNUSED(pvParameters);
	CmdProcessor_Enqueue(&XdkLive_EventQueue, HandleEvery10mS, NULL, UINT32_C(0));
}
