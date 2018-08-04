#include <BCDS_Basics.h>
#include <BSP_BoardType.h>
#include <XdkEvents.h>
#include <BCDS_BSP_Button.h>
#include <BCDS_CmdProcessor.h>
#include <BCDS_Retcode.h>
#include "XdkExceptions.h"

void ButtonOne_OnEvent(uint32_t data)
{
	if(data == BSP_XDK_BUTTON_PRESS) {
		Retcode_T retcode = CmdProcessor_enqueueFromIsr(&XdkLive_EventQueue, HandleEveryButtonOnePressed, NULL, data);
        if(retcode != RETCODE_OK)
        {
            Retcode_raiseErrorFromIsr(retcode);
        }
	}
}


Retcode_T SensorButton_one_Setup(void)
{
	return BSP_Button_Connect();

	return NO_EXCEPTION;
}

Retcode_T SensorButton_one_Enable(void)
{
	Retcode_T retcode = NO_EXCEPTION;

	retcode = BSP_Button_Enable((uint32_t) BSP_XDK_BUTTON_1, ButtonOne_OnEvent);
	if(retcode != NO_EXCEPTION) return retcode;


	return NO_EXCEPTION;
}
