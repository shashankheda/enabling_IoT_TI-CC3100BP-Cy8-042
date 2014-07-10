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

/*
 * Application Name     -   Getting started with Wi-Fi STATION mode
 * Application Overview -   This is a sample application demonstrating how to
 *                          start CC3100 in WLAN-Station mode and connect to a
 *                          Wi-Fi access-point. The application connects to an
 *                          access-point and ping's the gateway. It also checks
 *                          for an internet connectivity by pinging "www.ti.com"
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_Getting_Started_with_WLAN_Station
 *                          doc\examples\getting_started_with_wlan_station.pdf
 */

/**/
#include "simplelink.h"
#include "cli_uart.h"

#define APPLICATION_VERSION "1.0.0"

/**/
#define LOOP_FOREVER(line_number) \
            {\
                while(1); \
            }

#define ASSERT_ON_ERROR(line_number, error_code) \
            {\
                /* Handling the error-codes is specific to the application */ \
                if (error_code < 0) return error_code; \
                /* else, continue w/ execution */ \
            }

/*
 * Values for below macros shall be modified per the access-point's (AP) properties
 * SimpleLink device will connect to following AP when the application is executed
 */
#define SSID_NAME       "cc3200demo"         /* AP Name */
#define SEC_TYPE        SL_SEC_TYPE_OPEN    /* Security type (OPEN or WEP or WPA) */
#define PASSKEY         ""                  /* Password of the secured AP */

#define HOST_NAME       "www.ti.com"

/*
 * Values for below macros shall be modified for setting the 'Ping' properties
 */
#define PING_INTERVAL   1000    /* In msecs */
#define PING_TIMEOUT    3000    /* In msecs */
#define PING_PKT_SIZE   20      /* In bytes */
#define NO_OF_ATTEMPTS  3

#define SUCCESS         0

/* Status bits - These are used to set/reset the corresponding bits in a 'status_variable' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in a 'status_variable', the device is connected to the AP
                                 *      0 in a 'status_variable', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_AQUIRED,       /* If this bit is:
                                 *      1 in a 'status_variable', the device has acquired an IP
                                 *      0 in a 'status_variable', the device has not acquired an IP
                                 */

    STATUS_BIT_PING_DONE        /* If this bit is:
                                 *      1 in a 'status_variable', the device has completed the ping operation
                                 *      0 in a 'status_variable', the device has not completed the ping operation
                                 */
}e_StatusBits;

/* Application specific status/error codes */
typedef enum{
    LAN_CONNECTION_FAILED = -0x7D0,        /* Choosing this number to avoid overlap with host-driver's error codes */
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))

#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_AQUIRED)
#define IS_PING_DONE(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_PING_DONE)

/*
 * GLOBAL VARIABLES -- Start
 */
UINT32  g_Status = 0;
UINT32  g_PingPacketsRecv = 0;
UINT32  g_GatewayIP = 0;
/*
 * GLOBAL VARIABLES -- End
 */


/*
 * STATIC FUNCTION DEFINITIONS -- Start
 */
static INT32 configureSimpleLinkToDefaultState();
static INT32 establishConnectionWithAP();
static INT32 checkLanConnection();
static INT32 checkInternetConnection();
static INT32 initializeAppVariables();
static void displayBanner();
/*
 * STATIC FUNCTION DEFINITIONS -- End
 */


/*
 * ASYNCHRONOUS EVENT HANDLERS -- Start
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            sl_protocol_wlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                CLI_Write((unsigned char *)" Device disconnected from the AP on application's request \n\r");
            }
            else
            {
                CLI_Write((unsigned char *)" Device disconnected from the AP on an ERROR..!! \n\r");
            }
        }
        break;

        default:
        {
            CLI_Write((unsigned char *)" [WLAN EVENT] Unexpected event \n\r");
        }
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
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_ACQUIRED:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
            g_GatewayIP = pEventData->gateway;
        }
        break;

        default:
        {
            CLI_Write((unsigned char *)" [NETAPP EVENT] Unexpected event \n\r");
        }
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
                                  SlHttpServerResponse_t *pHttpResponse)
{
    /*
     * This application doesn't work with HTTP server - Hence these
     * events are not handled here
     */
    CLI_Write((unsigned char *)" [HTTP EVENT] Unexpected event \n\r");
}


/*!
    \brief This function handles ping report events

    \param[in]      pPingReport holds the ping report statistics

    \return         None

    \note

    \warning
*/
static void SimpleLinkPingReport(SlPingReport_t *pPingReport)
{
    SET_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = pPingReport->PacketsReceived;
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
    CLI_Write((unsigned char *)" [GENERAL EVENT] \n\r");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    /*
     * This application doesn't work w/ socket - Hence not handling these events
     */
    CLI_Write((unsigned char *)" [SOCK EVENT] Unexpected event \n\r");
}
/*
 * ASYNCHRONOUS EVENT HANDLERS -- End
 */


/*
 * Application's entry point
 */
int main(int argc, char** argv)
{
    INT32 retVal = -1;

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Stop WDT and initialize the system-clock of the MCU */
    stopWDT();
    initClk();

    /* Configure command line interface */
    CLI_Configure();

    displayBanner();

    /*
     * Following function configures the device to default state by cleaning
     * the persistent settings stored in NVMEM (viz. connection profiles &
     * policies, power policy etc)
     *
     * Applications may choose to skip this step if the developer is sure
     * that the device is in its default state at start of application
     *
     * Note that all profiles and persistent settings that were done on the
     * device will be lost
     */
    retVal = configureSimpleLinkToDefaultState();
    if(retVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == retVal)
            CLI_Write((unsigned char *)" Failed to configure the device in its default state \n\r");

        LOOP_FOREVER(__LINE__);
    }

    CLI_Write((unsigned char *)" Device is configured in default state \n\r");

    /*
     * Assumption is that the device is configured in station mode already
     * and it is in its default state
     */
    retVal = sl_Start(0, 0, 0);
    if ((retVal < 0) ||
        (ROLE_STA != retVal) )
    {
        CLI_Write((unsigned char *)" Failed to start the device \n\r");
        LOOP_FOREVER(__LINE__);
    }

    CLI_Write((unsigned char *)" Device started as STATION \n\r");

    /* Connecting to WLAN AP */
    retVal = establishConnectionWithAP();
    if(retVal < 0)
    {
        CLI_Write((unsigned char *)" Failed to establish connection w/ an AP \n\r");
        LOOP_FOREVER(__LINE__);
    }

    CLI_Write((unsigned char *)" Connection established w/ AP and IP is acquired \n\r");
    CLI_Write((unsigned char *)" Pinging...! \n\r");

    retVal = checkLanConnection();
    if(retVal < 0)
    {
        CLI_Write((unsigned char *)" Device couldn't connect to LAN \n\r");
        LOOP_FOREVER(__LINE__);
    }

    retVal = checkInternetConnection();
    if(retVal < 0)
    {
        CLI_Write((unsigned char *)" Device couldn't connect to the internet \n\r");
        LOOP_FOREVER(__LINE__);
    }

    CLI_Write((unsigned char *)" Device successfully connected to the LAN and internet \n\r");
    return 0;
}

/*!
    \brief This function configure the SimpleLink device in its default state. It:
           - Sets the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregisters mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static INT32 configureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};

    UINT8           val = 1;
    UINT8           configOpt = 0;
    UINT8           configLen = 0;
    UINT8           power = 0;

    INT32           retVal = -1;
    INT32           mode = -1;

    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(__LINE__, mode);

    /* If the device is not in station-mode, try configuring it in station-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(!IS_IP_AQUIRED(g_Status)) { _SlNonOsMainLoopTask(); }
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(__LINE__, retVal);

        retVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(__LINE__, retVal);

        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(__LINE__, retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status)) { _SlNonOsMainLoopTask(); }
    }

    /* Enable DHCP client*/
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    retVal = sl_Stop(0xFF);
    ASSERT_ON_ERROR(__LINE__, retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(__LINE__, retVal);

    return retVal; /* Success */
}

/*!
    \brief Connecting to a WLAN Access point

    This function connects to the required AP (SSID_NAME).
    The function will return once we are connected and have acquired IP address

    \param[in]  None

    \return     0 on success, negative error-code on error

    \note

    \warning    If the WLAN connection fails or we don't acquire an IP address,
                We will be stuck in this function forever.
*/
static INT32 establishConnectionWithAP()
{
    SlSecParams_t secParams = {0};
    INT32 retVal = 0;

    secParams.Key = PASSKEY;
    secParams.KeyLen = strlen(PASSKEY);
    secParams.Type = SEC_TYPE;

    retVal = sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Wait */
    while((!IS_CONNECTED(g_Status)) || (!IS_IP_AQUIRED(g_Status))) { _SlNonOsMainLoopTask(); }

    return SUCCESS;
}

/*!
    \brief This function checks the LAN connection by pinging the AP's gateway

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static INT32 checkLanConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    INT32 retVal = -1;

    CLR_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = 0;

    /* Set the ping parameters */
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_GatewayIP;

    /* Check for LAN connection */
    retVal = sl_NetAppPingStart( (SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                                 (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Wait */
    while(!IS_PING_DONE(g_Status)) { _SlNonOsMainLoopTask(); }

    if(0 == g_PingPacketsRecv)
    {
        /* Problem with LAN connection */
        return LAN_CONNECTION_FAILED;
    }

    /* LAN connection is successful */
    return SUCCESS;
}

/*!
    \brief This function checks the internet connection by pinging
           the external-host (HOST_NAME)

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static INT32 checkInternetConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    UINT32 ipAddr = 0;

    INT32 retVal = -1;

    CLR_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = 0;

    /* Set the ping parameters */
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_GatewayIP;

    /* Check for Internet connection */
    retVal = sl_NetAppDnsGetHostByName(HOST_NAME, sizeof(HOST_NAME), &ipAddr, SL_AF_INET);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Replace the ping address to match HOST_NAME's IP address */
    pingParams.Ip = ipAddr;

    /* Try to ping HOST_NAME */
    retVal = sl_NetAppPingStart( (SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                                 (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(__LINE__, retVal);

    /* Wait */
    while(!IS_PING_DONE(g_Status)) { _SlNonOsMainLoopTask(); }

    if (0 == g_PingPacketsRecv)
    {
        /* Problem with internet connection*/
        return INTERNET_CONNECTION_FAILED;
    }

    /* Internet connection is successful */
    return SUCCESS;
}

/*!
    \brief This function initializes the application variables

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static INT32 initializeAppVariables()
{
    g_Status = 0;
    g_PingPacketsRecv = 0;
    g_GatewayIP = 0;

    return SUCCESS;
}

/*!
    \brief This function displays the application's banner

    \param      None

    \return     None
*/
static void displayBanner()
{
    CLI_Write((unsigned char *)"\n\r\n\r");
    CLI_Write((unsigned char *)" Getting started with station application - Version ");
    CLI_Write((unsigned char *) APPLICATION_VERSION);
    CLI_Write((unsigned char *)"\n\r*******************************************************************************\n\r");
}
