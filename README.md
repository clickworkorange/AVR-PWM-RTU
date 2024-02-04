- <a href="#description">Description</a>
- <a href="#register-map">Register map</a>
- <a href="#avr-pin-map">AVR pin map</a>
- <a href="#defaults">Defaults</a>
- <a href="#crystal-selection">Crystal selection</a>
- <a href="#schematic">Schematic</a>
- <a href="#pcb-layout">PCB layout</a>
- <a href="#future">Future</a>
- <a href="#wetware-at-work">Wetware at work</a>

### Description
A Modbus and push-button controlled 4-channel PWM generator based on the Atmel ATMega328. Each channel can be set to one of four pre-defined duty cycles (0-255). In addition to being controllable by Modbus commands, the channel levels (0-3) can also be selected by means of individual push-buttons, wich increment a channel's level, wrapping at 3. The active level for each channel is output as a 2-bit value which is used to drive a set of four LEDs by means of a 74HC138 decoder. The idea is to allow for remote control of the individual channels over a three-wire interface (plus GND and VCC). The Modbus communication layer is provided by the <a href="https://github.com/mbs38/yaMBSiavr">yaMBSiavr</a> library. The code compiles to 9.3kB. 

*N.b. this is still work in progress and some features are yet to be implemented - see <a href="#future">future</a>.*

### Register map
Modbus register values are limited to a maximum value for each register; attempting to write a higher value than what is permitted for a given register will return a 03 ILLEGAL DATA VALUE response from the RTU. The same will happen if you try to write to a read only register. Writing a 1 to the last register (currently 50) will trigger a save to EEPROM of all register values - the RTU will revert to these stored values on reboot. 

#|Function|Value| |#|Function|Value
--|-------------|-----|-|--|-------------|-----
0|Ch0 Level|0-3| |26|Device ID 4-5|2 bytes
1|Ch1 Level|0-3| |27|Device ID 6-7|2 bytes
2|Ch2 Level|0-3| |28|Ch0 ID 0-1|2 bytes
3|Ch3 Level|0-3| |29|Ch0 ID 2-3|2 bytes
4|Ch0 Duty 0|0-255| |30|Ch0 ID 4-5|2 bytes
5|Ch0 Duty 1|0-255| |31|Ch0 ID 6-7|2 bytes
6|Ch0 Duty 2|0-255| |32|Ch1 ID 0-1|2 bytes
7|Ch0 Duty 3|0-255| |33|Ch1 ID 2-3|2 bytes
8|Ch1 Duty 0|0-255| |34|Ch1 ID 4-5|2 bytes
9|Ch1 Duty 1|0-255| |35|Ch1 ID 6-7|2 bytes
10|Ch1 Duty 2|0-255| |36|Ch2 ID 0-1|2 bytes
11|Ch1 Duty 3|0-255| |37|Ch2 ID 2-3|2 bytes
12|Ch2 Duty 0|0-255| |38|Ch2 ID 4-5|2 bytes
13|Ch2 Duty 1|0-255| |39|Ch2 ID 6-7|2 bytes
14|Ch2 Duty 2|0-255| |40|Ch3 ID 0-1|2 bytes
15|Ch2 Duty 3|0-255| |41|Ch3 ID 2-3|2 bytes
16|Ch3 Duty 0|0-255| |42|Ch3 ID 4-5|2 bytes
17|Ch3 Duty 1|0-255| |43|Ch3 ID 6-7|2 bytes
18|Ch3 Duty 2|0-255| |44|Slave Address|0-255
19|Ch3 Duty 3|0-255| |45|Baud Rate|0-7
20|Ch0-1 Invert|0-1| |46|Parity|0-2
21|Ch0-1 Scaler|0-7*| |47|Stop Bits|0-1
22|Ch2-3 Invert|0-1| |48|Firmware Revision|-
23|Ch2-3 Scaler|0-7*| |49|*Reserved*|
24|Device ID 0-1|2 bytes| |50|Save Settings|1/0
25|Device ID 2-3|2 bytes| | | |

\*) See the below table for the meaning of the scaler values:

**Value**|0|1|2|3|4|5|6|7
-----|----|----|----|----|----|----|----|----
**Scaler**|0|1|8|32|64|128|256|1024

Setting either of the scaler registers to zero disables those two channels. See <a href="#crystal-selection">Crystal selection</a> for the PWM frequencies resulting from the different prescalers. Note that at present changes to scalers, inversion and communication parameters requires rebooting the RTU for the changes to take effect. 

### AVR pin map

Pin|Port|Function| |Pin|Port|Function
---|----|-------------|-|---|----|-------------
1|PC6|Reset| |15|PB1|Ch0 PWM
2|PD0|RxD| |16|PB2|Ch1 PWM
3|PD1|TxD| |17|PB3|Ch2 PWM
4|PD2|Data Dir| |18|PB4|Ch3 Lvl LSB
5|PD3|Ch3 PWM| |19|PB5|Ch3 Lvl MSB
6|PD4|Load Defaults| |20|AVCC|+5V
7|Vcc|+5V| |21|AREF|n/c
8|GND|GND| |22|GND|GND
9|PB6|Xtal| |23|PC0|Ch0 Lvl LSB
10|PB7|Xtal| |24|PC1|Ch0 Lvl MSB
11|PD5|Ch2 Btn| |25|PC2|Ch0 Btn
12|PD6|Ch2 Lvl LSB| |26|PC3|Ch1 Lvl LSB
13|PD7|Ch2 Lvl MSB| |27|PC4|Ch1 Lvl MSB
14|PB0|Ch3 Btn| |28|PC5|Ch1 Btn

### Defaults
The default configuration values are: 

- Channel levels all set to 0
- Duty values for levels (all channels)
	- 0: 255
	- 1: 127
	- 2: 63
	- 3: 0
- Both scaler registers set to 4 (490 Hz @ 8 MHz)
- Communication parameters
	- Slave ID: 1
	- Baud: 9600
	- Parity: None
	- Stop bits: 1
 - Identification strings
 	- Device ID: "PWM-1"
  	- Channel IDs: "Ch0", "Ch1", "Ch2", "Ch3"

The RTU can be reset to these values at any time by holding PD4 low on power on / reset. 

### Crystal selection
It's important to pick a system clock frequency that results in the desired PWM frequency for your particular application. If you are controlling fans or other motor driven devices you'll probably want this to be outside human hearing range, if it's lights you'll want it to be high enough for persistence of vision to hide the flicker. Another consideration is the Baud-rate error that will result from choosing a system clock that is not evenly divisible by the desired communication speed. The table below shows the available PWM frequencies for a given clock and prescaler, with Baud-rate compatible clock speeds highlighted in bold: 

Xtal|1|8|32*|64|128*|256|1024
-----------|--------|--------|--------|--------|--------|--------|--------|
2.0000 MHz|7.8 kHz|980 Hz|245 Hz|123 Hz|61 Hz|31 Hz|8 Hz
**3.6864 MHz**|14.5 kHz|1.8 kHz|452 Hz|226 Hz|113 Hz|56 Hz|14 Hz
4.0000 MHz|15.7 kHz|2.0 kHz|490 Hz|245 Hz|123 Hz|61 Hz|15 Hz
**7.3728 MHz**|28.9 kHz|3.6 kHz|904 Hz|452 Hz|226 Hz|113 Hz|28 Hz
8.0000 MHz|31.4 kHz|3.9 kHz|980 Hz|490 Hz|245 Hz|123 Hz|31 Hz
16.0000 MHz|62.7 kHz|7.8 kHz|2.0 kHz|980 Hz|490 Hz|245 Hz|61 Hz
**18.4320 MHz**|72.3 kHz|9.0 kHz|2.3 kHz|1.1 kHz|565 Hz|282 Hz|71 Hz
20.0000 MHz|78.4 kHz|9.8 kHz|2.5 kHz|1.2 kHz|613 Hz|306 Hz|77 Hz

\*) Prescalers 32 & 128 are not available on Timer1, instead we get these by changing the TOP length to 10-bits / scaler 8 and 9-bits / scaler 64 respectively, and <a href="https://github.com/clickworkorange/AVR-PWM-RTU/blob/cd5e14912393d223fffba5abe3af9f1dc7a8ffb7/AVR/avr_pwm_rtu.c#L113-L127">scaling the 0-255 duty value</a> accordingly. 

### Schematic
The schematic has been updated to revision 0.2 and is now presentable - you can find it in [the /KiCAD folder](https://github.com/clickworkorange/AVR-PWM-RTU/tree/main/KiCAD). There will be more changes, not least on the input side, but this should work: 
![](https://github.com/clickworkorange/AVR-PWM-RTU/blob/main/Schematic.png?raw=true)
I will soon add the schematic for the remote control panels as well. 

### PCB layout
I've finished a PCB layout which is sized to fit in a 3U DIN-rail enclosure - it was a tight fit but it's in: 
![](https://github.com/clickworkorange/AVR-PWM-RTU/blob/main/PCB-front.png?raw=true)
![](https://github.com/clickworkorange/AVR-PWM-RTU/blob/main/PCB-back.png?raw=true)
![](https://github.com/clickworkorange/AVR-PWM-RTU/blob/main/PCB-3D-front.png?raw=true)
![](https://github.com/clickworkorange/AVR-PWM-RTU/blob/main/PCB-3D-back.png?raw=true)
I have yet to find a good source for multi-level screw terminals with a 3.5mm pitch, but I know they're out there somewhere! You can find the PCB layout together with the schematic in [the /KiCAD folder](https://github.com/clickworkorange/AVR-PWM-RTU/tree/main/KiCAD). 

### Future
#### Must:
* Make comms parameters settable through Modbus registers.
#### Should:
* *nothing here right now*
#### Could:
* Apply PWM frequency scalers without reboot.
* Smooth level transitions. 
* Add additional functions for long-press and double-clicks.
* Make the PCB fit in a DIN-rail enclosure.
#### Won't:
* Make PWM frequency continuously variable.
* Replace Modbus with a different protocol. 

### Wetware at work
Resistance may indeed be futile, and I for one welcome our new data-based overlords - who I'm sure are paying close attention. But this software was written by a *human*, and only humans can enjoy writing code. Perhaps that joy by itself will one day be seen as an act of <a href="https://en.wikipedia.org/wiki/Joy_as_an_Act_of_Resistance">resistance</a>?

<img src="https://raw.githubusercontent.com/clickworkorange/KivySightGlass/main/human_coder.png" alt="Wetware at work" width="128" height="128" />
