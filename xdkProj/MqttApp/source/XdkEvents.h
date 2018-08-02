#ifndef XDKLIVE_EVENTS_H
#define XDKLIVE_EVENTS_H

#include <BCDS_Retcode.h>
#include <BCDS_CmdProcessor.h>
#include <BCDS_Basics.h>


extern CmdProcessor_T XdkLive_EventQueue;

//Retcode_T HandleEvery10mS();
Retcode_T HandleEvery10mS(void* userParameter1, uint32_t userParameter2);

Retcode_T HandleEveryButtonOnePressed(void* userParameter1, uint32_t userParameter2);
#endif
