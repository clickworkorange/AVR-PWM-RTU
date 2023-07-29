- <a href="#description">Description</a>
- <a href="#register-map">Register map</a>
- <a href="#goals">Goals</a>
- <a href="#simulation">Simulation</a>
- <a href="#wetware-at-work">Wetware at work</a>

### Description
A Modbus and push-button controlled 4-channel PWM generator based on the Atmel ATMega 328. Each channel can be set to one of four pre-defined duty cycles (0-255). In addition to being controllable by Modbus commands, the channel levels (0-3) can also be selected by means of individual push-buttons, wich increment a channel's level, wrapping at 3. The active level for each channel is output as a 2-bit value which is used to drive a set of four LEDs by means of a 74HC138 decoder. The idea is to allow for remote control of the individual channels over a three-wire interface (plus GND and VCC). The Modbus communication layer is provided by the <a href="https://github.com/mbs38/yaMBSiavr">yaMBSiavr</a> library. 

*N.b. this is very much work in progress and many planned features are yet to be implemented - see <a href="#goals">goals</a>.*

### Register map
Putting a 1 into the last register (currently 50) will trigger a write to EEPROM of all register values. 

![Registers](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Registers.png)

### Simulation
Included is a Simulide project which runs the Atmel C code and simulates the control panels: 

![Simulide](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Simulide.png)

### Goals
* Design the full circuit and create PCB layouts in KiCAD
* Enforce limits on register values and validate input parameters
* Make slave address & comms parameters settable through Modbus registers
* Incorporate a reset button to return the EEPROM to factory defaults
* Make PWM phase and frequency (or scaler) settable through Modbus registers
* Simplify port configuration & port reset code 

### Wetware at work
Resistance may indeed be futile, and I for one welcome our new software based overlords - who I'm sure are paying close attention. But this software was written by a *human*, and only humans can enjoy writing code. Perhaps that joy by itself will one day be seen as an act of <a href="https://en.wikipedia.org/wiki/Joy_as_an_Act_of_Resistance">resistance</a>?

<img src="https://raw.githubusercontent.com/clickworkorange/KivySightGlass/main/human_coder.png" alt="Wetware at work" width="128" height="128" />
