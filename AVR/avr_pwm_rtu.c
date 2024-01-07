/*
             |||
            (o o)
 +-------oOO-{_}-OOo--------+
 |                          |
 |   © 2024 Ola Tuvesson    |
 |   clickworkorange Ltd    |
 |                          |
 | info@clickworkorange.com |
 |                          |
 +--------------------------+

*/

#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include "avr_pwm_rtu.h"
#include "yaMBSiavr.h"

volatile uint8_t instate = 0;
volatile uint8_t outstate = 0;
volatile uint16_t EEMEM eeprom[REGCOUNT];

static uint16_t registers[REGCOUNT];
static uint16_t rxreg, rxval, rxcnt; 

static uint8_t const BTN0 = 0b00000100; //PORTC
static uint8_t const BTN1 = 0b00100000; //PORTC
static uint8_t const BTN2 = 0b00100000; //PORTD
static uint8_t const BTN3 = 0b00000001; //PORTB
static uint8_t const BTND = 0b00010000; //PORTD

static uint8_t const LED0 = 0b00000011; //PORTC
static uint8_t const LED1 = 0b00011000; //PORTC
static uint8_t const LED2 = 0b11000000; //PORTD
static uint8_t const LED3 = 0b00110000; //PORTB

static uint8_t const PWM0 = 0b00000010; //PORTB
static uint8_t const PWM1 = 0b00000100; //PORTB
static uint8_t const PWM2 = 0b00001000; //PORTB
static uint8_t const PWM3 = 0b00001000; //PORTD

// TODO: this could be more readable... 
static uint8_t const T1WG[] = {0,_BV(WGM10),_BV(WGM10),_BV(WGM10)|_BV(WGM11),_BV(WGM10),_BV(WGM11),_BV(WGM10),_BV(WGM10)};
static uint8_t const T2WG[] = {0,_BV(WGM20)|_BV(WGM21),_BV(WGM20)|_BV(WGM21),_BV(WGM20)|_BV(WGM21),_BV(WGM20)|_BV(WGM21),_BV(WGM20)|_BV(WGM21),_BV(WGM20)|_BV(WGM21),_BV(WGM20)|_BV(WGM21)};
static uint8_t const T1CS[] = {0,_BV(CS10),_BV(CS11),_BV(CS11),_BV(CS10)|_BV(CS11),_BV(CS10)|_BV(CS11),_BV(CS12),_BV(CS10)|_BV(CS12)};
static uint8_t const T2CS[] = {0,_BV(CS20),_BV(CS21),_BV(CS20)|_BV(CS21),_BV(CS22),_BV(CS20)|_BV(CS22),_BV(CS21)|_BV(CS22),_BV(CS20)|_BV(CS21)|_BV(CS22)};

#define OUT(port, led, level) (port ^= (port ^ level) & (led));

void loadRegisters(void) {
	read_register_array(eeprom, registers, REGCOUNT * 2);
}

void saveRegisters(void) {
	if(registers[REGCOUNT-1] == 1) {
		// the last register is the save flag, reset it
		registers[REGCOUNT-1] = 0;
		write_register_array(eeprom, registers, REGCOUNT * 2);
	}
}

void setDefaults(void) {
	if((PIND & BTND) == 0) {
		// if BTND (PD4) is held low, load defaults and save to EEPROM
		memcpy(registers, DEFAULT, REGCOUNT * 2);
		saveRegisters();
	}
}

void pinSetup(void) { 
	// output on B4,B5 (LEDs ch3) & B1,B2,B3 (PWM ch0-2)
	  DDRB = LED3 | PWM0 | PWM1 | PWM2;
	// output on C0,C1 & C3,C4 (LEDs ch0 & 1)
	  DDRC = LED0 | LED1;
	// output on D6,D7 (LEDs ch2) & D3 (PWM ch3)
	  DDRD = LED2 | PWM3;

	// pull-up on B0 (button input ch3)
	 PORTB = BTN3;
	// pull-up on C2,C5 (button inputs ch0 & 1)
	 PORTC = BTN0 | BTN1;
	// pull-up on D5,D4 (button inputs ch2 & defaults)
	 PORTD = BTN2 | BTND;
}

void comSetup(void) {
	modbusSetAddress(registers[44]);
}

void clockSetup(void) {
	// UART clock on Timer0
	TCCR0B = _BV(CS01); // prescaler 8
	TIMSK0 = _BV(TOIE0); // enable overflow interrupt

	// Timer1 (on B1,B2)
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | T1WG[registers[21]];
	TCCR1B = _BV(WGM12) | T1CS[registers[21]];

	// Timer2 (on B3,D3)
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | T2WG[registers[23]];
	TCCR2B = T2CS[registers[23]];
}

ISR(TIMER0_OVF_vect) { 
	modbusTickTimer();
}

int timerOneLength(int duty) {
	// adjust Timer1 duty for TOP length
	if(TCCR1A & _BV(WGM11)) {
		if(TCCR1A & _BV(WGM10)) {
			// 10-bits (e.g. 127 = 511)
			return duty<<2;
		} else {
			// 9-bits (e.g. 127 = 255)
			return duty<<1;
		}
	} else {
		// 8-bits (no change)
		return duty;
	}
}

void setLevel(uint8_t channel, uint8_t level) {
	registers[channel] = level;
	switch(channel) {
		case 0:
			OCR1A = timerOneLength(registers[4 + level]);
			OUT(PORTC, LED0, registers[0]);
		break;
		case 1:
			OCR1B = timerOneLength(registers[8 + level]);
			OUT(PORTC, LED1, registers[1]<<3);
		break;
		case 2:
			OCR2A = registers[12 + level];
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				// protect PORTD writes from interrupts
				OUT(PORTD, LED2, registers[2]<<6);
			}
		break;
		case 3:
			OCR2B = registers[16 + level];
			OUT(PORTB, LED3, registers[3]<<4);
		break;
	}
}

void updateLevels(void) {
	for(int i = 0; i < CHANNELS; i++) {
		setLevel(i, registers[i]);
	}
}

void incrChannel(uint8_t channel) {
	setLevel(channel, (registers[channel] + 1) % LEVELS);
}

int validate(uint16_t reg, uint16_t val) {
	if(val < (1<<REGSIZE[reg])) {
		// check if register is read only
		return REGSIZE[reg] != 0; 
	} else {
		return 0;
	}
}

void modbusGet(void) {
	if (modbusGetBusState() & (1<<ReceiveCompleted)) {
		switch(rxbuffer[1]) {
			case fcReadHoldingRegisters: {
				modbusExchangeRegisters(registers,0,REGCOUNT);
				break;
			}
			case fcPresetSingleRegister: {
				rxreg = (rxbuffer[2]<<8) | rxbuffer[3];
				rxval = (rxbuffer[4]<<8) | rxbuffer[5];
				if(validate(rxreg, rxval) == 0) {
					modbusSendException(ecIllegalDataValue);
				}
				modbusExchangeRegisters(registers,0,REGCOUNT);
				updateLevels();
				saveRegisters();
				break;
			}
			case fcPresetMultipleRegisters: {
				rxreg = (rxbuffer[2]<<8) | rxbuffer[3];
				rxcnt = (rxbuffer[4]<<8) | rxbuffer[5];
				for(int i = 0; i < rxcnt; i+=2) {
					rxval = (rxbuffer[7+i]<<8) | rxbuffer[8+i];
					if(validate(rxreg+i, rxval) == 0) {
						modbusSendException(ecIllegalDataValue);
						break;
					} 
				}
				modbusExchangeRegisters(registers,0,REGCOUNT);
				updateLevels();
				saveRegisters();
				break;
			}
			default:
				modbusSendException(ecIllegalFunction);
				break;
		}
	}
}

int main(void) {
	pinSetup();
	setDefaults();
	loadRegisters();
	clockSetup();
	comSetup();
	sei();
	modbusInit();
	wdt_enable(7);
	updateLevels();

	// TODO: make PWM phase configurable
	// TODO: make PWM frequency configurable
	// TODO: make comm parameters configurable
	while(1) {
		wdt_reset();
		modbusGet();
		if((PINC & BTN0) == 0) {
			incrChannel(0);
			while((PINC & BTN0) == 0); // Wait until the button is released
		}
		if((PINC & BTN1) == 0) {
			incrChannel(1);
			while((PINC & BTN1) == 0);
		}
		if((PIND & BTN2) == 0) {
			incrChannel(2);
			while((PIND & BTN2) == 0);
		}
		if((PINB & BTN3) == 0) {
			incrChannel(3);
			while((PINB & BTN3) == 0);
		}
	}
}