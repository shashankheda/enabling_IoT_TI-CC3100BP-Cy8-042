CC3100 Flash Programmer - v0.5
--------------------------------------
Release date: 		
		May 15, 2014


Programming:
		To program the device you need to use the SimpleLink programmer tool.
		The SimpleLink Programmer is a small command line utility that allows to download new images to a SimpleLink device.
		From command line window, run the following command:
		
			slprog_2.0.7.0.31.0.0.4.2.1.5.3.3_SEC.exe -p PPP 

		Where:
			PPP is the first comm port connected to the SimpleLink device

Warning:
		Downloading the new package will erase any data already exists on your SFLASH including IP and MAC configurations, certificates etc.
		Please take care to download the configuration files again afterwards.