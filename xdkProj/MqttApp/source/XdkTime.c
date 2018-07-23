#include <FreeRTOS.h>
#include <BCDS_Basics.h>
#include <BCDS_CmdProcessor.h>
#include <timers.h>
#include <BCDS_Retcode.h>
#include "XdkTime.h"
#include "XdkExceptions.h"
#include "XdkEvents.h"

static TimerHandle_t timerHandleEvery10mS;

static void InternalHandleEvery1Second1(TimerHandle_t xTimer)
{
	BCDS_UNUSED(xTimer);
	CmdProcessor_Enqueue(&XdkLive_EventQueue, timerHandleEvery10mS, NULL, 0);
}


Retcode_T SetupTime(void)
{

	timerHandleEvery10mS = xTimerCreate("timerHandleEvery1Second1", UINT32_C(1000), pdTRUE, NULL, InternalHandleEvery1Second1);

	if(timerHandleEvery10mS == NULL)
	{
		return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_OUT_OF_RESOURCES);
	}

	return NO_EXCEPTION;
}

Retcode_T EnableTime(void)
{
	Retcode_T result = NO_EXCEPTION;


	result = EveryHandleEvery1Second1_Enable();
	if(result != NO_EXCEPTION)
	{
		return result;
	}

	return NO_EXCEPTION;
}

Retcode_T EveryHandleEvery1Second1_Enable(void)
{
	xTimerStart(timerHandleEvery10mS, 0);

	return NO_EXCEPTION;
}
