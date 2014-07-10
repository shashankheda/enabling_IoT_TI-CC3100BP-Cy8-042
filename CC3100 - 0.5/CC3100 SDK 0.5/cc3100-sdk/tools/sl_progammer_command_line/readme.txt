SimpleLink Programmer - v0.5
--------------------------------------
Release date: 		
		May 15, 2014

Programming:
		The SimpleLink Programmer tool is a shim command line utility that allows to download new content to a SimpleLink device and also provides other functionalities like erase and format.
		There are two options the tool provides:
		1) SDK flasher: use a batch file for programming other than the package binaries
			From command line window, run the following command:
		
				sl_prog.exe -p <UART port number> -b <batch file> [-v]
				
				where verbose mode is optional and denoted as -v
				
			The available commands in the batch file are listed below:
			1) File deletion
			
				ERASE,<filenmae>
				where:
					<filename> can be any file name that has already been flashed to the device. 
					For example: ERASE,/main.html
						
			2) Storage format
			
				FORMAT,<storage device>
				where:
					<storage device> can be one of the followings: [FLASH, SFLASH]
					
			3) File download
			
				<source filename>,<target filename>,<storage device>
				where:
					<source filename> can be any file name (located in the same directory of the tool)
					<target filename> filename in the device file system
					<storage device> can be one of the followings: [FLASH, SFLASH]


Enterprise certificate files:

	Need to add the followings to the batch file:
	1) <path to enterprise CA certificate>,/cert/ca.pem,SFLASH
	2) <path to enterprise client certificate>,/cert/client.pem,SFLASH
	3) <path to enterprise private key>,/cert/private.key,SFLASH

SSL certificate files:
	
	There are two options for flashing SSL/TLS related files:
	1) use any filename. These filenames should be used via setsockopt()
	2) use filename in the following syntax: /cert/%d.der where %d denotes a file number. The file number should be used via setsockopt()
	
	For example, if the 2nd option is used and a CA certificate is to be used, it is required to add the following line to the batch file:
		<path to SSL/TLS CA certificate>,/cert/128.der,SFLASH
	Then, in the application code, the following lines need to be added:
		SlSockSecureFiles_t    sockSecureFiles;
		
		sockSecureFiles.secureFiles[0] = 0;
		sockSecureFiles.secureFiles[1] = 0;
		sockSecureFiles.secureFiles[2] = 128;
		sockSecureFiles.secureFiles[3] = 0;
		retVal = sl_SetSockOpt(rawSocket, SL_SOL_SOCKET, SL_SO_SECURE_FILES, sockSecureFiles.secureFiles, sizeof(sockSecureFiles));
			
HTML web pages:

	If desired to add a file for use with the internal HTTP server, the files are added as any other file.
	
	For example, if the main.html page is to be replaced with a user page, the following line needs to be added to the batch file:
			<path to user main.html>,/main.html,SFLASH

Warning:
		Downloading the new package will erase any data already exists on your SFLASH including IP and MAC configurations, certificates etc.
		Please take care to download the configuration files again afterwards.
