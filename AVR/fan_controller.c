#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "yaMBSiavr.h"

// why is BAUD definition here ignored by #ifndef in yaMBSiavr.h?
// #define BAUD 9600L

#define channels 4
#define regsize 20
#define read_register_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
#define write_register_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)

volatile uint8_t instate = 0;
volatile uint8_t outstate = 0;
volatile uint16_t EEMEM eeprom[regsize];
volatile uint16_t registers[regsize];

static uint8_t const BTN0 = 0b00000100; //PORTC
static uint8_t const BTN1 = 0b00100000; //PORTC
static uint8_t const BTN2 = 0b00100000; //PORTB
static uint8_t const BTN3 = 0b00000001; //PORTD

void loadRegisters(void) {
	read_register_array(eeprom, registers, regsize * 2);
}
void saveRegisters(void) {
	write_register_array(eeprom, registers, regsize * 2);
}

void pinSetup(void) { 
	// output on B4,B5 (LEDs ch4) & B1,B2,B3 (PWM ch1-3)
	  DDRB |= 0b00111110;
	// output on C0,C1 & C3,C4 (LEDs ch1 & 2)
	  DDRC |= 0b00011011;
	// output on D6,D7 (LEDs ch3) & D3 (PWM ch4)
	  DDRD |= 0b11001000;
	// pull-up on B0 (button input ch4)
	 PORTB |= BTN3;
	// pull-up on C2,C5 (button inputs ch1 & 2)
	 PORTC |= BTN0 | BTN1;
	// pull-up on D5 (button input ch3)
	 PORTD |= BTN2;

	// UART clock on timer0
	TCCR0B |= (1<<CS01); //prescaler 8
	TIMSK0 |= (1<<TOIE0);

	// 8-bit Fast PWM on timer1 (on B1,B2)
	TCCR1A |= _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
	//TCCR1B |= _BV(CS20) | _BV(WGM12); // 31.25 KHz (prescaler 1)
	TCCR1B |= _BV(CS12) | _BV(WGM12); // 30.52 Hz (prescaler 256)

	// 8-bit Fast PWM on timer2 (on B3,D3)
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
	//TCCR2B = _BV(CS20); // 31.25 KHz (prescaler 1)
	TCCR2B = _BV(CS21) | _BV(CS22); // 30.52 Hz (prescaler 256)
}

ISR(TIMER0_OVF_vect) { 
	modbusTickTimer();
}

void setLevel(uint8_t channel, uint8_t level) {
	registers[channel] = level;
	switch(channel) {
		case 0: {
			OCR1A = registers[4 + level];
			PORTC = (registers[0] | (registers[1]<<3)) | BTN0 | BTN1;
		}
		break;
		case 1: {
			OCR1B = registers[8 + level];
			PORTC = (registers[0] | (registers[1]<<3)) | BTN0 | BTN1;
		}
		break;
		case 2: {
			OCR2A = registers[12 + level];
			PORTD = registers[2]<<6 | BTN2;
		}
		break;
		case 3: {
			OCR2B = registers[16 + level];
			PORTB = registers[3]<<4 | BTN3;
		}
		break;
	}
}

void updateLevels(void) {
	for(int i = 0; i < channels; i++) {
		setLevel(i, registers[i]);
	}
}

void incrChannel(uint8_t channel) {
	uint8_t new_level = (registers[channel] + 1) % 4;
	setLevel(channel, new_level);
}

void modbusGet(void) {
	if (modbusGetBusState() & (1<<ReceiveCompleted)) {
		switch(rxbuffer[1]) {
			case fcReadHoldingRegisters: {
				modbusExchangeRegisters(registers,0,regsize);
			}
			break;
			case fcPresetSingleRegister: {
				modbusExchangeRegisters(registers,0,regsize);
				updateLevels();
				saveRegisters();
			}
			break;
			case fcPresetMultipleRegisters: {
				modbusExchangeRegisters(registers,0,regsize);
				updateLevels();
				saveRegisters();
			}
			break;
			default: {
				modbusSendException(ecIllegalFunction);
			}
			break;
		}
	}
}

int main(void) {
	loadRegisters();
	pinSetup();
	sei();
	modbusSetAddress(0x01);
	modbusInit();
	wdt_enable(7);
	updateLevels();

	// TODO: do not write EEPROM for registers 0-3
	// TODO: assemble LED pins into a byte that can be written to? virtual port?
	// TODO: simplify PORTC = (levels[0] | (levels[1]<<3)) | BTN0 | BTN1;
	// TODO: enforce limits on register values
	// TODO: make PWM phase configurable
	// TODO: make PWM frequency configurable
	// TODO: make slave address configurable
	// TODO: make comm parameters configurable
	// TODO: move some things to a header file
	// TODO: button debouncing (idelly non-blocking)
	// TODO: add reset button (factory defaults)
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