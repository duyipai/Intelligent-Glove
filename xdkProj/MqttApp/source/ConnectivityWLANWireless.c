#include <BCDS_Basics.h>
#include <Serval_Ip.h>
#include <Serval_Network.h>
#include <BCDS_NetworkConfig.h>
#include <BCDS_WlanConnect.h>
#include <PAL_socketMonitor_ih.h>
#include <inttypes.h>
#include <PAL_initialize_ih.h>
#include <BCDS_Retcode.h>
#include "XdkExceptions.h"

#define NETWORK_SSID "duyipai"
#define NETWORK_PSK  "duyipai999"

Retcode_T ConnectivityWLANWireless_Setup(void)
{
	retcode_t servalRetcode = PAL_initialize();
	if(RC_OK != servalRetcode)
	{
		printf("[ERROR, %s:%d] PAL initialize failed: %d\n", __FILE__, __LINE__, servalRetcode);
		return EXCEPTION_EXCEPTION;
	}

	return NO_EXCEPTION;
}

Retcode_T ConnectivityWLANWireless_Enable(void)
{
	Retcode_T retcode;

	retcode = WlanConnect_Init();
	if(RETCODE_OK != retcode)
	{
		return retcode;
	}

	/* The order of calls is important here. WlanConnect_init initializes the CC3100 and prepares
	 * its future use. Calls to NetworkConfig_ fail if WlanConnect_Init was not called beforehand.
	 */
	retcode = NetworkConfig_SetIpDhcp(NULL);
	if (RETCODE_OK != retcode)
	{
		return retcode;
	}

	printf("[INFO, %s:%d] Connecting to %s\n", __FILE__, __LINE__, NETWORK_SSID);
	/* Passing NULL as onConnection callback (last parameter) makes this a blocking call, i.e. the
	 * WlanConnect_WPA function will return only once a connection to the WLAN has been established,
	 * or if something went wrong while trying to do so. If you wanted non-blocking behavior, pass
	 * a callback instead of NULL. */
	retcode = WlanConnect_WPA((WlanConnect_SSID_T) NETWORK_SSID, (WlanConnect_PassPhrase_T) NETWORK_PSK, NULL);
	if(RETCODE_OK != retcode)
	{
		return retcode;
	}

	NetworkConfig_IpSettings_T currentIpSettings;
	retcode = NetworkConfig_GetIpSettings(&currentIpSettings);
	if(RETCODE_OK != retcode)
	{
		return retcode;
	}
	else
	{
		uint32_t ipAddress = Basics_htonl(currentIpSettings.ipV4);

		char humanReadableIpAddress[SERVAL_IP_ADDR_LEN] = { 0 };
		int conversionStatus = Ip_convertAddrToString(&ipAddress, humanReadableIpAddress);
		if (conversionStatus < 0)
		{
			printf("[WARNING, %s:%d] Couldn't convert the IP address to string format\n", __FILE__, __LINE__);
		}
		else
		{
			printf("[INFO, %s:%d] Connected to WLAN. IP address of this device is: %s\n", __FILE__, __LINE__, humanReadableIpAddress);
		}
	}

	PAL_socketMonitorInit();

	return RETCODE_OK;

	return NO_EXCEPTION;
}


