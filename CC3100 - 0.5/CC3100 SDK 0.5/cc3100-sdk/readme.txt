================================================================================
Package content for CC3100 SDK v0.5 release
Release Date: May 15, 2014
================================================================================

docs:
        - examples: Contains document w/ details on each sample application
        - hardware: Contains hardware specific 'User-Guides' and schematics for 
          CC3100 BoosterPack and FTDI Emulation Board
        - simplelink_api: Contains Simplelink-API guide for the host-driver
        - Contains a 'Quick Start Guide' with instructions on setting up the 
          platform and executing sample application(s)
          
        
examples: 
        Contains the sample applications for SDK v0.5 - Each application has a 
        README file w/ links to the collaterals with detailed instructions
	- antenna_selection: A sample application to configure antenna selection
	  GPIO and connect to AP using antenna with better signal strength.
        - connection_policies: A sample application to demonstrate the usage of
          CC3100's profiles and connection-policies
        - email: A sample application which sends a pre-configured email at the
          push of a button or a user-configured email through the CLI
        - enterprise_nework_connection: A sample application to connect the
          enterprise network using secure certificate
        - file_operations: A sample application to demonstrate the use if file-
          operation APIs of host-driver        
        - get_time: A sample application to demonstrate connection scenario
          with SNTP server and requesting for time information
        - get_weather: A sample application to demonstrate connection scenario
          with 'openweathermap.org' and requesting for weather information
        - getting_started_with_wlan_ap: A sample application to configure 
          CC3100 in AP mode. The application verifies the connection by pinging
          the client that gets connected to it
        - getting_started_with_wlan_station: A sample application to configure
          CC3100 in STA mode. The application verifies the connection by pinging
          the AP.
        - http_server: A sample application to show the capability of CC3100 
          acting as a web server
        - ip_configuration: A sample application that connects to an AP using staitic 
          IP address  
        - mdns: A sample application to register and broadcase services on mdns port  
        - mode_config: A sample application to demonstrate the switching of 
	  CC3100 operation mode from STA to AP and vice-versa
        - nwp_filters: A sample application to demonstrate the management of 
          Rx-filtering process
	- nwp_power_policy: A sample application to demonstrate the switching between
	  different power modes of CC3100.
	- out_of_box: This sample application demonstrates Out-of-Box experience with 
	  CC3100 Booster Pack  
	- p2p: A sample application to configure the CC3100 in p2p mode and connect
	  to remote p2p device.
	- provisioning_ap: A sample application with html pages to demonstrate the
	  usage of internal http for Wi-Fi provisioning with CC31xx
        - provisioning_smartconfig: A sample application to demonstrate the 
          usage of TI's SmartConfig™ Wi-Fi provisioning technique
        - provisioning_wps: A sample application to demonstrate the usage of WPS
          Wi-Fi provisioning with CC31xx
        - scan_policy: A sample application to demonstrate the setting of scan-
          policy in CC3100.
        - sls_email*: A sample application which sends an email to user-configured 
          email-address through the CLI 
        - sls_get_time*: A sample application to demonstrate connection scenario
          with SNTP server and requesting for time information
        - sls_get_weather*: A sample application to demonstrate connection scenario
          with 'openweathermap.org' and requesting for weather information
	- sls_getting_started_with_wlan_ap*: A sample application to configure 
          CC3100 in AP mode. The application verifies the connection by pinging
          the client that gets connected to it
        - sls_getting_started_with_wlan_station*: A sample application to configure
          CC3100 in STA mode. The application verifies the connection by pinging
          the AP.
        - sls_sniffer_with_filters*: A sample application that provides a 
          functional example to highlight the ability of using a raw socket to 
          read packets received by CC31xx receives, as well as how to filter the 
          packets it reads from the MAC level.
        - sls_transceiver_mode*: A sample application that uses CC31xx
          to send and receive data over a raw socket, which is not restricted
          by protocol-specific formatting in the way that TCP or UDP sockets 
          are. The demo also highlights the built in ability to collect 
          statistics regarding the packets received by CC31xx.        
        - sls_xmpp_client:  sample application which demonstrates the connection
          scenario with an XMPP server
        - spi_debug_tool: A diagnostics application for checking the SPI
          configuration
        - ssl: A sample application to demonstrate the using of certificate with
          SSL
        - tcp_socket: A sample application to demonstrate the connection 
          scenario and  basic TCP functionality.
        - transceiver_mode: A sample application to demonstrate CC3100's transceiver 
	          mode of operation
        - udp_socket: A sample application to demonstrate the connection 
          scenario and  basic UDP functionality               
        - xmpp_client: A sample application which demonstrates the connection
          scenario with an XMPP server
        *These are SimpleLink-Studio applications - SimpleLink-Studio is a 
         tool to aid in the development of applications designed to work 
         with the SimpleLink WiFi CC31xx family of wireless chips. 
         Using SimpleLink-Studio, code can be written and run in a desktop IDE,
         such as Visual Studio or Eclipse.

                      
platforms: 
        Contains all the supported platforms in SDK pre v0.5
        - msp430f5529-launchpad
        	- Contains CCS projects for all the sample applications that 
        	  are supported on this platform
	        - Contains IAR projects for 'getting_started_with_wlan_station'
	          and 'getting_started_with_wlan_ap' application                  
	        - Contains all the required drivers
		- Supports UART and SPI interface.
	        - Contains 'user.h' (Details in the abstract @ 
                  <cc3100_sdk>/simplelink
        - tiva c launchpad
        	- Contains CCS and IAR projects for 'getting_started_with_wlan_station'
	          and 'getting_started_with_wlan_ap' application
	        - Contains all the required drivers
		- Supports UART and SPI interface.
	        - Contains 'user.h' (Details in the abstract @ 
                  <cc3100_sdk>/simplelink
        - msp430fr5739: 
                - Contains CCS & IAR projects for 'getting_started_with_wlan_station'
	          and 'getting_started_with_wlan_ap' application
                - Contains all the required drivers
		- Supports SPI interface.
                - Contains 'user.h' (Details in the abstract @ 
                  <cc3100_sdk>/simplelink
        - msp430fr5529: 
                - Contains IAR and CCS projects for 'getting_started_with_wlan_station'
	          and 'getting_started_with_wlan_ap' application
                - Contains all the required drivers
		- Supports SPI interface.
                - Contains 'user.h' (Details in the abstract @ 
                  <cc3100_sdk>/simplelink
        - simplelinkstudio:
                - Contains Visual-Studio and Eclipse projects for the sample 
                  applications that are supported on this platform
		- Supports UART and SPI interface.
                - Contains 'user.h' (Details in the abstract @ 
                  <cc3100_sdk>/simplelink
         
         
simplelink:
        - Contains 'SimpleLink Host Driver' code. The package contains a 
          template 'template_user.h' file which should be filled by the user 
          for his specific application and platform.
         
         
tools:
        - cc31xx_board_drivers: Drivers to be installed on Host-PC
        - smartphone_apps: Applications
        - sl_progammer_command_line: 'sl_prog' utility for flashing certificates 
          and html pages
