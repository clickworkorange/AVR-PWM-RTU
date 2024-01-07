- <a href="#description">Description</a>
- <a href="#register-map">Register map</a>
- <a href="#avr-pin-map">AVR pin map</a>
- <a href="#crystal-selection">Crystal selection</a>
- <a href="#schematic">Schematic</a>
- <a href="#simulation">Simulation</a>
- <a href="#goals">Goals</a>
- <a href="#wetware-at-work">Wetware at work</a>

### Description
A Modbus and push-button controlled 4-channel PWM generator based on the Atmel ATMega 328. Each channel can be set to one of four pre-defined duty cycles (0-255). In addition to being controllable by Modbus commands, the channel levels (0-3) can also be selected by means of individual push-buttons, wich increment a channel's level, wrapping at 3. The active level for each channel is output as a 2-bit value which is used to drive a set of four LEDs by means of a 74HC138 decoder. The idea is to allow for remote control of the individual channels over a three-wire interface (plus GND and VCC). The Modbus communication layer is provided by the <a href="https://github.com/mbs38/yaMBSiavr">yaMBSiavr</a> library. The code compiles to 8.7kB. 

*N.b. this is very much work in progress and many planned features are yet to be implemented - see <a href="#goals">goals</a>.*

### Register map
Modbus register values are limited to a maximum value for each register; attempting to write a higher value than what is permitted for a given register will return a 03 ILLEGAL DATA VALUE response from the RTU. The same will happen if you try to write to a read only register. Writing a 1 to the last register (currently 50) will trigger a save to EEPROM of all register values - the RTU will revert to these stored values on reboot. 

Register|Function|Value
--------|----------|-----
0|Ch0 Level|0-3
1|Ch1 Level|0-3
2|Ch2 Level|0-3
3|Ch3 Level|0-3
4|Ch0 Duty 0|0-255
5|Ch0 Duty 1|0-255
6|Ch0 Duty 2|0-255
7|Ch0 Duty 3|0-255
8|Ch1 Duty 0|0-255
9|Ch1 Duty 1|0-255
10|Ch1 Duty 2|0-255
11|Ch1 Duty 3|0-255
12|Ch2 Duty 0|0-255
13|Ch2 Duty 1|0-255
14|Ch2 Duty 2|0-255
15|Ch2 Duty 3|0-255
16|Ch3 Duty 0|0-255
17|Ch3 Duty 1|0-255
18|Ch3 Duty 2|0-255
19|Ch3 Duty 3|0-255
.|...|.
50|Save Settings|0/1

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

### Crystal selection
It's important to pick a system clock frequency that results in the desired PWM frequency for your particular application. If you are controlling fans or other motor driven devices you'll probably want this to be outside human hearing range, if it's lights you'll want it to be high enough for persistence of vision to hide the flicker. Another consideration is the Baud-rate error that will result from choosing a system clock that is not evenly divisible by the desired communication speed. The table below shows the available PWM frequencies for a given clock and prescaler, with Baud-rate compatible clock speeds highlighted in bold: 

Xtal|1|8|64|256|1024
------------|--------|--------|--------|--------|--------|
2.0000 MHz|7.8 kHz|980 Hz|123 Hz|31 Hz|8 Hz
**3.6864 MHz**|14.5 kHz|1.8 kHz|226 Hz|56 Hz|14 Hz
4.0000 MHz|15.7 kHz|2.0 kHz|245 Hz|61 Hz|15 Hz
**7.3728 MHz**|28.9 kHz|3.6 kHz|452 Hz|113 Hz|28 Hz
8.0000 MHz|31.4 kHz|3.9 kHz|490 Hz|123 Hz|31 Hz
16.0000 MHz|62.7 kHz|7.8 kHz|980 Hz|245 Hz|61 Hz
**18.4320 MHz**|72.3 kHz|9.0 kHz|1.1 kHz|282 Hz|71 Hz
20.0000 MHz|78.4 kHz|9.8 kHz|1.2 kHz|306 Hz|77 Hz


### Schematic
A schematic can be found in the KiCAD folder. This is still work in progress. 

![Schematic](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Schematic.png)

### Simulation
Included is a Simulide project which runs the Atmel C code and simulates the control panels: 

![Simulide](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Simulide.png)

### Goals
* Finish KiCAD schematic and design a PCB from it
* Make slave address & comms parameters settable through Modbus registers
* Make PWM phase and frequency (or scaler) settable through Modbus registers

### Wetware at work
Resistance may indeed be futile, and I for one welcome our new data-based overlords - who I'm sure are paying close attention. But this software was written by a *human*, and only humans can enjoy writing code. Perhaps that joy by itself will one day be seen as an act of <a href="https://en.wikipedia.org/wiki/Joy_as_an_Act_of_Resistance">resistance</a>?

<img src="https://raw.githubusercontent.com/clickworkorange/KivySightGlass/main/human_coder.png" alt="Wetware at work" width="128" height="128" />
