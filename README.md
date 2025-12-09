# MiniPOD
The MiniPOD is a small package that we constructed to run an OPC-R2 on an UNO QWIIC on 3V logic and also to log to an SD shield &amp; to take timestamps with an RTC.

# Update Tracker
Thanks to Izzy for this suggestion! Here we will be tracking each version of the firmware.
| Version       | "Named" Ver.   | Pilot         | Date               | Description & Purpose                		|
| ------------- | -------------- | ------------- | -------------      | ----------------------------------------------- |
| V3.1.0    	| Initial Upload | Percy         | June 19, 2024   | IDK just don't use this one |
| V3.1.2        | Primary Firmware      | Percy         | June 20, 2024   |  Uses LEDs, writes all OPC data, RTC Timestamp	|
| V3.2.0        | Initial Fix	 | Percy         | May 7, 2025   | File naming updated, changed clock adjust convention	|
| V3.2.1        | Lot of Memory  | Percy         | May 8, 2025   | Fixes 3.2.0 issue creating files (probID "forgor")	|
| V3.3.1        | USE THIS ONE | Percy         | Dec 9, 2025   | Optimizes memory by changing .print files	|

## Memory Change Notes
### V3.2.1
Sketch uses 24686 bytes **(76%)** of program storage space. Maximum is 32256 bytes. \n
Global variables use 1648 bytes **(80%)** of dynamic memory, leaving 400 bytes for local variables. Maximum is 2048 bytes.
### V3.3.1
Sketch uses 22368 bytes **(69%)** of program storage space. Maximum is 32256 bytes. \n
Global variables use 1302 bytes **(63%)** of dynamic memory, leaving 746 bytes for local variables. Maximum is 2048 bytes.
