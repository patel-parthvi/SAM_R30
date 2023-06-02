
Date -> 15-12-2022 :

1 - Started ADC readings on 2 channels, one for battery voltage and another for CAN resistance value.

2 - Added Battery symbol and battery voltage in device info page of OLED.

3 - Added CAN Shorting test with resistance value. Resistance value will be displayed on OLED.



Date -> 05-01-2023 : 

1 - Got working MCP2515 over SPI.

2 - Started CAN Communication with CAN Sensors for Multiple nodes.

3 - Implememted GUI for upto 6 CAN sensors.

4 - User interface has been added, in which user can select any CAN sensor from display using UP/Down button and on selecting it LED blinking will 		start on that particular sensor for 30 seconds.

5 - Added CAN 24V Enable/Disable functionality to protect the board from getting damaged when short circuit test is performed. During CAN short 		circuit test CAN 24V output will be disabled for the same purpose. 



Date -> 12-01-2023 : 

1 - Added Battery Percentage in Device info page.

2 - Started Getting CAN sensor info from multiple nodes (upto 9 Nodes).

3 - Implemented GUI for CAN sensors upto 9 nodes in scan for CAN sensors page of OLED in order to display MAC address and Antenna type of all CAN sensors.



Date -> 02-02-2023:

1 - CAN Sensor LED Turn ON/OFF changes has been done.

2 - Basic communication of MiWi tester with wireless hub has been implemented.

3 - Firmware modified to keep 24V OFF when performing CAN Short circuit test.

4 - Implemented UART communication for debug purpose (MiWi logs can be viewed from UART debug connector).


Date -> 16-03-2023

1 - Implemented OTA Firmware upgrage over MiWi.

2 - Resolved bugs of CANH-CANL doen't gets cleared after displaying SHORT and Resistance value does not update from 120 to 60, it shows 160 on display.



Date -> 22-03-2023

1 - Made Configurable MiWi PAN ID.

2 - Added CAN fill circle and clear status functionalities.

Note : Here we have hard coded Band = 2 and Channel = 2 for MiWi tester.