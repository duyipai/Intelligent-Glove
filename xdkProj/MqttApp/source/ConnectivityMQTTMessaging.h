#ifndef CONNECTIVITYWLAN_WLAN_H
#define CONNECTIVITYWLAN_WLAN_H

#include <BCDS_Retcode.h>


/**
 * Sets up the WLAN.
 */
Retcode_T ConnectivityWLANWireless_Setup(void);

/**
 * Enables the WLAN sensor.
 */
Retcode_T ConnectivityWLANWireless_Enable(void);



#endif
