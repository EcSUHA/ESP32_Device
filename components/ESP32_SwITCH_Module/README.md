# ESP32_SwITCH_Module
## for SCDE (Smart Connected Device Engine) V2 only !

* ESP32 -> intended for use on ESP32 SoC hardware only ! 

* SwITCH -> SwITCH Feature. It is used to drive GPIOs as a switch (on/off/pwm) with the goal to control hardware
(Relay, LED-Controller, Smart Socket, ...). The ESP32 SoC GPIO will be connected to the ESP32 PWM-Hardware.
The PWM-Hardware is used and controlled by this Module! This is hardware PWM/switching only!

* Module -> intended for use with SCDE (Smart Connected Device Engine) only !

---


Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany, for EcSUHA.de

MSchulze780@GMAIL.COM

Copyright by EcSUHA

This is part of

- ESP 8266EX & ESP32 SoC Activities ...

- HoME CoNTROL & Smart Connected Device Engine Activities ...
 
EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE

---
 
### Debug & testing hardware setup:
Connect an 8 channel SSR Relay board to ESP32 GPIOs 13,12,14,27,26,25,33,32

Prepare the configuration by the following commands and store it to the resulting makerfile

define SSR.1.at.GPIO.13 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.2.at.GPIO.12 esp32_SwITCH GPIO=12&BLOCK=Low_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.3.at.GPIO.14 esp32_SwITCH GPIO=14&BLOCK=High_Speed&CHANNEL=1&TIMER=1&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.4.at.GPIO.27 esp32_SwITCH GPIO=27&BLOCK=High_Speed&CHANNEL=2&TIMER=1&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.5.at.GPIO.26 esp32_SwITCH GPIO=26&BLOCK=High_Speed&CHANNEL=3&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.6.at.GPIO.25 esp32_SwITCH GPIO=25&BLOCK=High_Speed&CHANNEL=4&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.7.at.GPIO.33 esp32_SwITCH GPIO=33&BLOCK=High_Speed&CHANNEL=5&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

define SSR.8.at.GPIO.32 esp32_SwITCH GPIO=32&BLOCK=High_Speed&CHANNEL=6&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55

//possible GPIOs 0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16, 17, 18 ,19, 21, 22, 23, 25, 26, 27, 32 and 33

//checked GPIOs OK 0, 12, 13, 14, 25, 26, 27


---

# Setting up an ESP32_SwITCH by the Maker

An ESP32_SwITCH can be configured by the maker by define command.
Overview and options (includes required Key=Values -> Keys that should get values in an define command)

Help-String implemented in module:


define MySwitch ESP32_SwITCH GPIO=0|1|2|3|4|5|12|13|14|15|16|17|18|19|21|22|23|25|26|27|32|33&PWM_BLOCK=High_Speed|Low_Speed&CHANNEL=0|1|2|3|4|5|6|7&TIMER=0|1|2|3&DUTY=0&HPOINT=0&SIG_OUT_EN=Disabled|Enabled&IDLE_LV=Low|High&RESOLUTION=0&TICK_SOURCE=REF|APB_SLOW&FREQ_HZ=[0-9]

required Keys are listed in help-string.
optional Keys:
-/-
forbidden Keys:
-/-

Example for setting up an ESP32_SwITCH:

Init custom SwITCH Feature named "Switch.1" at GPIO 13:


define Switch.1 ESP32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=Low&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
 

---

# Description of Keys and Values parsed by ESP32_SwITCH

GPIO=0|1|2|3|4|5|12|13|14|15|16|17|18|19|21|22|23|25|26|27|32|33
-> Verbindet den PWM-Kanal mit dem angegebenen GPIO (inklusive der Konfiguration)
-> Connects the PWM-Channel with the given GPIO (including the setup)
-> checked GPIOs OK 0, 12, 13, 14, 25, 26, 27

PWM_BLOCK=High_Speed|Low_Speed

CHANNEL=0|1|2|3|4|5|6|7
-> Setzt den Hardware-PWM-Kanal des ESP32 (0-7) der verwendet werden soll
-> Sets the hardware-pwm-channel of the ESP32 (0-7) that should be used

TIMER=0|1|2|3
-> Setzt den Hardware-Timer für den gewählten PWM-Kanal des ESP32 (0-3) der verwendet werden soll
-> Sets the hardware-timer for the choosen pwm-channel of the ESP32 (0-3) that should be used

DUTY=0 Zahl?
-> Setzt die Leistung der PWM für den zugewiesenen Kanal
-> Sets the duty of the PWM for the assigned channel

HPOINT=0 (Dezimal Zahl vom Typ uint20_t)
-> Setzt den Einschaltpunkt (High-Level-Point) (max. 20 Bit) der PWM für den zugewiesenen Kanal
-> Sets the switch-on-point (High-Level-Point) (max. 20 Bit) for the assigned channel

SIG_OUT_EN=Disabled|Enabled
-> Verbindet / schaltete das PWM Signal auf den Ausgang (GPIO) durch
-> Connects / switches the PWM Signal through the output GPIO

IDLE_LV=Low|High
-> Setzt den Ausgangspegel am GPIO (Low|High) wenn SIG_OUT_EN=Disabled
-> Sets the output level at the GPIO (Low|High) when SIG_OUT_EN=Disabled

RESOLUTION=0 (Dezimal Zahl vom Typ uint20_t) ODER ANZAHL BITS 0-20?
-> Setzt die Bitbreite (max. 20 Bit) des PWM Timers
-> Sets the Bit length (max. 20 Bit) of the PWM timer

TICK_SOURCE=REF|APB_SLOW
-> Setzt den Takterzeuger 0: REF_TICK / 1: APB_CLK
-> Sets the clock source 0: REF_TICK / 1: APB_CLK 
(REF=1.000.000hz, APB=80.000.000 hz)

FREQ_HZ=[0-9]	// NOTE: clc bases on .RESOLUTION, .TICK_SOURCE !! (should be parsed here)
-> Konfiguriert den Divisionsfaktor
-> Configures the division factor for the divider in high-speed timer from given frequency in HZ



---



  HELP-STRING:
  Define <Def-Name> ESP32_SwITCH <GPIO=> <CHANNEL=> <TIMER=> <TICK_SOURCE=REF|APB_SLOW> <RESOLUTION=8> <FREQ_HZ=> <IDLE_LV=>[<SIG_OUT_EN=> <HPOINT=0> <DUTY=0>]
 * Set <Def-Name> <GPIO=> <TICK_SOURCE=> <RESOLUTION=8> <FREQ_HZ=> <SIG_OUT_EN=> <IDLE_LV=> <HPOINT=0> <DUTY=>
 * (not allowed to change by SET: <BLOCK=> <TIMER=> <CHANNEL=> )
 *
 
 
 
 ---
 
 SAMPLE USAGE :
 Init custom SwITCH Feature named "Switch.1" at GPIO 13:
 define Switch.1 esp32_SwITCH GPIO=13&BLOCK=High_Speed&CHANNEL=0&TIMER=0&TICK_SOURCE=Low&RESOLUTION=8&FREQ_HZ=9765&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
 
 set Switch.1 DUTY=210
 
 
 




---

# How to get information about an defined ESP32_SwITCH

Enter list command with name:

list SSR.8.at.GPIO.32

You will get an overview of the definition:

Common:
  DEF        GPIO=32&BLOCK=High_Speed&CHANNEL=6&TIMER=2&TICK_SOURCE=REF&RESOLUTION=8&FREQ_HZ=16&SIG_OUT_EN=Enabled&IDLE_LV=Low&HPOINT=0&DUTY=55
  NAME       SSR.8.at.GPIO.32
  NR         11
  Readings:
  2019-01-28 18:58:04 | Duty = 55
  2019-01-28 18:58:04 | Resolution = 256
  STATE       2018.11.3
  TYPE       ESP32_SwITCH
Attributes:

Only the Common-Definition is currently shown!

---





x

