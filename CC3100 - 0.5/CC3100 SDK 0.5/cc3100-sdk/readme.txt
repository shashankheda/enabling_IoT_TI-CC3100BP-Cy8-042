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
        - getting_started_with_wlan_station: A sample application to configure
          CC3100 in STA mode. The application verifies the connection by pinging
          the AP.
        - spi_debug_tool: A diagnostics application for checking the SPI
          configuration
       
                      
platforms: 
        Contains all the supported platforms in SDK pre v0.5
        - cypresscy8-042
        	- Contains all the required drivers
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
