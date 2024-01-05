- <a href="#description">Description</a>
- <a href="#register-map">Register map</a>
- <a href="#avr-pin-map">AVR pin map</a>
- <a href="#schematic">Schematic</a>
- <a href="#simulation">Simulation</a>
- <a href="#goals">Goals</a>
- <a href="#wetware-at-work">Wetware at work</a>

### Description
A Modbus and push-button controlled 4-channel PWM generator based on the Atmel ATMega 328. Each channel can be set to one of four pre-defined duty cycles (0-255). In addition to being controllable by Modbus commands, the channel levels (0-3) can also be selected by means of individual push-buttons, wich increment a channel's level, wrapping at 3. The active level for each channel is output as a 2-bit value which is used to drive a set of four LEDs by means of a 74HC138 decoder. The idea is to allow for remote control of the individual channels over a three-wire interface (plus GND and VCC). The Modbus communication layer is provided by the <a href="https://github.com/mbs38/yaMBSiavr">yaMBSiavr</a> library. The code compiles to 8.7kB. 

*N.b. this is very much work in progress and many planned features are yet to be implemented - see <a href="#goals">goals</a>.*

### Register map
Writing a 1 to the last register (currently 50) will trigger a write to EEPROM of all register values. 

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
Only a sinlge i/o pin (PD4) remains unused. 

Pin|Port|Function
---|----|----------
1|PC6|Reset
2|PD0|RxD
3|PD1|TxD
4|PD2|Data Dir
5|PD3|PWM #3
6|PD4|n/c
7|Vcc|+5V
8|GND|GND
9|PB6|Xtal
10|PB7|Xtal
11|PD5|Btn #2
12|PD6|Lvl #2 LSB
13|PD7|Lvl #2 MSB
14|PB0|Btn #3
15|PB1|PWM #0
16|PB2|PWM #1
17|PB3|PWM #2
18|PB4|Lvl #3 LSB
19|PB5|Lvl #3 MSB
20|AVCC|+5V
21|AREF|n/c
22|GND|GND
23|PC0|Lvl #0 LSB
24|PC1|Lvl #0 MSB
25|PC2|Btn #0
26|PC3|Lvl #1 LSB
27|PC4|Lvl #1 MSB
28|PC5|Btn #1

### Schematic
A schematic can be found in the KiCAD folder. This is still work in progress. 

![Schematic](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Schematic.png)

### Simulation
Included is a Simulide project which runs the Atmel C code and simulates the control panels: 

![Simulide](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Simulide.png)

### Goals
* Finish KiCAD schematic and design a PCB from it
* Make slave address & comms parameters settable through Modbus registers
* Incorporate a reset button to return the EEPROM to factory defaults
* Make PWM phase and frequency (or scaler) settable through Modbus registers

### Wetware at work
Resistance may indeed be futile, and I for one welcome our new data-based overlords - who I'm sure are paying close attention. But this software was written by a *human*, and only humans can enjoy writing code. Perhaps that joy by itself will one day be seen as an act of <a href="https://en.wikipedia.org/wiki/Joy_as_an_Act_of_Resistance">resistance</a>?

<img src="https://raw.githubusercontent.com/clickworkorange/KivySightGlass/main/human_coder.png" alt="Wetware at work" width="128" height="128" />
