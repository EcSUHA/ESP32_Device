# ESP32 Device (template)
### is an platform template (ESP-IDF/ESP32) used to start creating custom IoT Devices running the

# SCDE
## Smart Connected Device Engine - Generation 2

### The new tool for makers to create IoT Devices

Makers can quickly build a firmware for their IoT-Devices
based on the Smart Connected Device Engine core extended by 
  * included commands and modules
  * own commands and modules
  * third party commands and modules
  
Programmers can provide
  * commands and modules for use in Makers IoT-Devices
  
IoT Devices setup is stored in the maker.cfg, IoT Devices state is stored in the state.cfg

Embedded Commands (SCDE-Core)
  * Attr
  * Define
  * Delete
  * Deleteattr
  * Get
  * Help
  * Include
  * IOWrite
  * List
  * Rename
  * Rereadcfg
  * Save
  * Set
  * Setstate 
  * Shutdown

Embedded Modules (SCDE-Core)
  * Global

Embedded Commands (linked in build process, ESP32-Platform)
  * -/-

Embedded Modules (linked in build process, ESP32-Platform)
  * Telnet (maintained as Module-Template)
  * ESP32_DeVICE (maintained as Module-Template)
    
Additional Commands (loadable module, ESP32-Platform)
  * loadable modules not supported on platform

Additional Modules (loadable module, ESP32-Platform)
  * loadable modules not supported on platform

Additional EcSUHA Modules on request (for testing purposes only)
  * ESP32_BH1750       Experimental
  * ESP32_Control      To control the ESP32
  * ESP32_I2C_Master   Experimental
  * ESP32_S0           For S0 counting
  * ESP32_SwITCH       For switchig and PWM with internal ESP32 hardware
 
  
---


Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE


---


Recommended installation and use:

1. Install + Test ESP-IDF (We use home/ESP32 folder)
prepare path (may be required)
. $HOME/ESP32/esp-idf/export.sh

2. clone ESP32_Device into this folder, can use custom name - recurse-submodules

$ git clone --recurse-submodules https://github.com/EcSUHA/LINUX_Device.git

3. To setup the project / make changes to ESP32 IDF (do not change anything)
idf.py menuconfig

4. Build the image
$ idf.py build

5. Flash the image
$ idf.py -p /dev/ttyUSB0 flash

6. Build the SPIFFS image:
./mkspiffs -c filesystem/ -b 4096 -p 256 -s 0x100000 spiffs.bin

7. Flash the SPIFFS Image:
esptool --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash -z 0x110000 spiffs.bin

8. view debug output
sudo microcom -p /dev/ttyUSB0 -s 115200
#(EXIT mit strg $ $) also doppelt

9. Connect via telnet (Putty) on port 1234 (according to maker.cfg file)

---


Notes:

CMD to clone SCDE (ESP-IDF should be installed and tested)
git clone --recursive <project url>


CMD to build the partition table:
make partition_table
(read screen for instructions anout flashing the table!)


