# Shear Stress Bioreactor
	Shear Stress Bioreactor project for the University of Idaho 2024 - 2025 senior capstone project.

# Students involved:

	* Gibson Moseley (CS)
	* Dillon West (BE)
	* Nathan Nelson (BE)

# Value Proposition
	Tendon-related injuries impact millions of people and collectively cost millions of dollars in medical expenses every year. Patients are left permanently debilitated in many cases, due to the poor natural healing capacity of tendon tissue. More research into cell mechanobiology is needed to both better understand the causes of such injuries and devise better means of treatment, and experimentation is best performed in an in vitro environment that can be carefully controlled, such as in a bioreactor. Unfortunately, existing bioreactor systems cost tens of thousands of dollars per unit, making them inaccessible to many researchers. Our goal is to design and build an easy-to-use and low-cost shear stress bioreactor that can deliver fluid shear stress to cells in culture without the need for overly expensive equipment. We will produce a fully operable prototype and instructions to replicate it, so it can be created for use by other labs.

# Repository Overview
	- The software for the project is developed for the ESP32 microcontroller with Visual Studio Code using [PlatformIO.](https://platformio.org/)
	- Forked off last years team project and used some of their previous code for project basis, located at https://github.com/CJ5518/ShearStressBioreactor

# ToDo Goals
	- Overhaul Website - Make it look nicer and better laid out (bootstrap)
	- Add password protection for routines (one time use)
    - Make pump use decimal/float speeds rather than only integers

# Setup Instructions
	1. Make sure to download the necessary communication drivers "CP210x USB to UART Bridge VCP Drivers" located at https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads. This allows for communication between the ESP32 and your computer.

	2. You will need to upload all this code onto your ESP32 Microcontroller.
    	2.1. Make sure when uploading and you see the connection message within the terminal that you press the "BOOT" button on the ESP32 the first time to make sure it is able to receive data.

    	2.2. Using platformIO, upload the file system image by clicking on the PlatformIO Icon and selecting "Upload Filesystem Image" under the "Platform" folder.

    	2.3. Then upload all the ESP32 code using the PlatformIO upload button.

	3. Next you will need to setup wifi communication between your router and the ESP32, after you finish uploading your code for the first time you will need to go into your wifi setting on your device and connect to "BioCapstoneESP". Then select your router from the list of devices and input the password. The ESP32 will then attempt to connect to the router. In the vscode terminal it will display what IP address the ESP32 is hosting to on that router. That is the website address you will need to connect to run the Bioreactor. You can set an IP address you want by going to the "WebHosting.hpp" file and setting it in the "wifiManager.setSTAStaticIPConfig()" function in the "initWebSetup()" function.
    	3.1 More setting and instructions can be found at https://github.com/tzapu/WiFiManager

	4. Now you can run the ESP32 Bioreactor by connection to the IP address on your router and adding all your appropriate settings. If you wish data to be saved on a MicroSD card make sure to slot one in that is formatted and no greater than 16gb.