### Atmel-PWM-RTU
A Modbus and push-button controlled 4-channel PWM generator based on the Atmel ATMega 328. Each channel can be set to one of four pre-defined duty cycles (0-255). In addition to being controllable by Modbus commands, the channel levels (0-3) can also be set by means of individual push-buttons, wich increment a channel's level, wrapping at 3. The active level for each channel is output as a 2-bit value which is used to drive a set of four LEDs by means of a 74HC138 decoder. 

### Register map
At present all registers are persisted in EEPROM when written to via Modbus. Channel levels set via push-buttons do not persist. 

![Registers](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Registers.png)

### Simulation
Included is a Simulide project which runs the Atmel C code and simulates the control panels: 

![Simulide](https://raw.githubusercontent.com/clickworkorange/Atmel-PWM-RTU/main/Simulide.png)

### Wetware at work
Resistance may indeed be futile, and I for one welcome our new software based overlords - who I'm sure are paying close attention. But this software was written by a *human*, and only humans can enjoy writing code. Perhaps that joy by itself will one day be seen as an act of <a href="https://en.wikipedia.org/wiki/Joy_as_an_Act_of_Resistance">resistance</a>?

<img src="https://raw.githubusercontent.com/clickworkorange/KivySightGlass/main/human_coder.png" alt="Wetware at work" width="128" height="128" />
