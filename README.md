Features:
	- Plug&Play design
	- Configure Ethernet interface(dhcp/static)
	- Configure WiFi interface
	- Configure Modem interface(Qualcomm modem)
	- Address reservation
	- Provide internet access to connected users
	- Access security
		Connected users need to provide credentials to use internet
	- Web UI for configurations
	- Traffic splitter
		Specific connections to go through specific internet sources. Split the traffic depending on IP/URL/PORT connection
	- VPN support
	- Switching between different internet sources loosing minimal packages
		Really fast switching between different internet sources depending on priority.
	- Switching between different internet sources without any lost packages
		Double connection to special cloud server. This way when an internet connection fails the other one would continue working.
	- Firewall/Traffic Filter
		Allow/Deny connections depending on MAC/PORT/IP/PROTOCOL
	- Output traffic limitation
		Traffic limitation if internet plan is with limited internet access
	- Users traffic limitation
		Limit users who are using this device internet.
	- Users internet speed limitation and priorities
		Limit the max speed that given user can use
	- Internet access time
		Set duration for which internet is available for specific user
	- Caching input
		Online pages for users
	- Caching output - Only output user
		Output data that failed and send when internet connection is back. Do not report bad internet connection to the user.
	- USB Printer support
		


Build Procedure
	
	mkdir build

	cd build

	cmake -D CMAKE_TOOLCHAIN_FILE=../Toolchain-RaspberryPi.cmake -D INSTALL_PATH=/app/ ..

	make install

	cd flash

	tar -czvf nm.tar.gz *

	scp nm.tar.gz pi@192.168.0.210:~/

	ssh pi@192.168.0.210 

	sudo su 
	
	cd /

	mkdir /app
	
	tar -xzvf /home/pi/nm.tar.gz -C /app

	cp -r /app/etc /

	systemctl enable nm 

	systemctl start nm


	 cd .. && make install -j12 && cd flash && rm -rf nm.tar.gz && tar -czvf nm.tar.gz * && scp nm.tar.gz pi@192.168.0.210:~/
