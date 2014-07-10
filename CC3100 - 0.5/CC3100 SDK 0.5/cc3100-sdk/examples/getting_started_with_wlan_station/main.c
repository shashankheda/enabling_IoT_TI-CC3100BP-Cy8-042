/*
 * main.c - Sample application to switch to STA mode, connect and ping AP
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "simplelink.h"

#define SSID_NAME        "cc3200demo"          /* Open AP name to connect to. */

#define PING_INTERVAL     1000
#define PING_TIMEOUT      3000
#define PING_SIZE         20
#define NO_OF_ATTEMPTS    3

#define CONNECTION_STATUS_BIT   0
#define IP_AQUIRED_STATUS_BIT   1
#define IP_LEASED_STATUS_BIT    2
#define PING_DONE_STATUS_BIT    3
typedef enum {
	CONNECTED = 0x01, IP_AQUIRED = 0x02, IP_LEASED = 0x04, PING_DONE = 0x08

} e_Status;
UINT8 g_Status = 0;

unsigned int g_PingPacketsRecv = 0;

/*!
 \brief This function handles WLAN events

 \param[in]      pWlanEvents is the event passed to the handler

 \return         None

 \note

 \warning
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvents) {
	switch (pWlanEvents->Event) {
	case SL_WLAN_CONNECT_EVENT:
		g_Status |= CONNECTED;
		break;

	case SL_WLAN_DISCONNECT_EVENT:
		g_Status &= ~(1 << CONNECTION_STATUS_BIT | 1 << IP_AQUIRED_STATUS_BIT);
		break;

	default:
		break;
	}
}

/*!
 \brief This function handles events for IP address acquisition via DHCP
 indication

 \param[in]      pNetAppEvent is the event passed to the handler

 \return         None

 \note

 \warning
 */
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
	switch (pNetAppEvent->Event) {
	case SL_NETAPP_IPV4_ACQUIRED:
		g_Status |= IP_AQUIRED;
		break;

	default:
		break;
	}
}

/*!
 \brief This function handles callback for the HTTP server events

 \param[in]      pServerEvent - Contains the relevant event information
 \param[in]      pServerResponse - Should be filled by the user with the
 relevant response information

 \return         None

 \note

 \warning
 */
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
		SlHttpServerResponse_t *pHttpResponse) {
	/* Unused in this application */
}

/*!
 \brief This function handles ping report events

 \param[in]      pPingReport holds the ping report statistics

 \return         None

 \note

 \warning
 */
static void SimpleLinkPingReport(SlPingReport_t *pPingReport) {
	g_Status |= PING_DONE;
	g_PingPacketsRecv = pPingReport->PacketsReceived;
}

/*!
 \brief Connecting to a WLAN Access point

 This function connects to the required AP (SSID_NAME).
 This code example assumes the AP doesn't use WIFI security.
 The function will return once we are connected and have acquired IP address

 \param[in]  None

 \return     None

 \note

 \warning    If the WLAN connection fails or we don't aquire an IP address,
 We will be stuck in this function forever.
 */
static void WlanConnect() {
	SlSecParams_t secParams;

	secParams.Key = "";
	secParams.KeyLen = 0;
	secParams.Type = SL_SEC_TYPE_OPEN;

	sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);

	while ((0 == (g_Status & CONNECTED)) || (0 == (g_Status & IP_AQUIRED))) {
		_SlNonOsMainLoopTask();
	}
}

int main(void) {
	SlPingStartCommand_t PingParams;
	SlPingReport_t Report;
	_NetCfgIpV4Args_t ipV4;

	unsigned char len = sizeof(_NetCfgIpV4Args_t);
	;
	unsigned char IsDHCP = 0;
	INT32 mode = ROLE_STA;
	UINT32 IpAddr = 0;
	int Status = 0;

	/* Stop WDT */
	stopWDT();

	/* Initialize the system clock of MCU */
	initClk();

	/* Initializing the CC3100 device */
	mode = sl_Start(0, 0, 0);
	if (ROLE_STA != mode) {
		if (ROLE_AP == mode)
			/* We need to wait for this event if in AP mode before doing
			 anything else */
			while (0 == (g_Status & IP_AQUIRED))
				_SlNonOsMainLoopTask();

		/* If some other application has configured the device in AP/P2P,
		 then change to STA */
		sl_WlanSetMode(ROLE_STA);

		g_Status = 0;
		sl_Stop(0x00FF);

		/* Delay of 1 second */
		Delay(1000);
		sl_Start(0, 0, 0);
	}

	/* Connecting to WLAN AP - Set with static parameters defined at the top
	 After this call we will be connected and have IP address */
	WlanConnect();

	/* Read the IP parameter */
	sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &IsDHCP, &len,
			(unsigned char *) &ipV4);

	/* Set the ping parameters */
	PingParams.PingIntervalTime = PING_INTERVAL;
	PingParams.PingSize = PING_SIZE;
	PingParams.PingRequestTimeout = PING_TIMEOUT;
	PingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
	PingParams.Flags = 0;
	PingParams.Ip = ipV4.ipV4Gateway; /* Fill the GW IP address, which is our AP address */

	/* Check for LAN connection */
	sl_NetAppPingStart((SlPingStartCommand_t*) &PingParams, SL_AF_INET,
			(SlPingReport_t*) &Report, SimpleLinkPingReport);

	while (0 == (g_Status & PING_DONE)) {
		_SlNonOsMainLoopTask();
	}

	g_Status &= ~(1 << PING_DONE_STATUS_BIT);

	if (!g_PingPacketsRecv) {
		/* Problem with LAN connection */
		return -1;
	}

	g_PingPacketsRecv = 0;

	/* We are successfully able to ping AP */
	/* Check for Internet connection */
	/* Querying for ti.com IP address */
	Status = sl_NetAppDnsGetHostByName("www.ti.com", 10, &IpAddr, SL_AF_INET);
	if (Status < 0) {
		/* LAN connection is successful */
		/* Problem with Internet connection*/
		return -2;
	}

	/* Replace the ping address to match ti.com IP address */
	PingParams.Ip = IpAddr;

	/* Try to ping www.ti.com */
	sl_NetAppPingStart((SlPingStartCommand_t*) &PingParams, SL_AF_INET,
			(SlPingReport_t*) &Report, SimpleLinkPingReport);
	while (0 == (g_Status & PING_DONE)) {
		_SlNonOsMainLoopTask();
	}

	if (g_PingPacketsRecv) {
		/* LAN connection is successful */
		/* Internet connection is successful */
		g_PingPacketsRecv = 0;
		return 0;
	} else {
		/* LAN connection is successful */
		/* Problem with Internet connection*/
		return -2;
	}
}
