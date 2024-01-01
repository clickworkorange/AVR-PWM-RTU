/*
             |||
            (o o)
 +-------oOO-{_}-OOo--------+
 |                          |
 |   © 2023 Ola Tuvesson    |
 |   clickworkorange Ltd    |
 |                          |
 | info@clickworkorange.com |
 |                          |
 +--------------------------+

*/

// set fuses with 
// sudo avrdude -p atmega328 -P usb -c dragon_isp -v -U lfuse:w:0xff:m -U hfuse:w:0xd1:m -U efuse:w:0xff:m 
// 8MHz ext. crystal, no divide by 8, preserve EEPROM 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include "avr_pwm_rtu.h"
#include "yaMBSiavr.h"

volatile uint8_t instate = 0;
volatile uint8_t outstate = 0;
volatile uint16_t EEMEM eeprom[REGSIZE];
volatile uint16_t registers[REGSIZE];

static uint8_t const BTN0 = 0b00000100; //PORTC
static uint8_t const BTN1 = 0b00100000; //PORTC
static uint8_t const BTN2 = 0b00100000; //PORTB
static uint8_t const BTN3 = 0b00000001; //PORTD

static uint8_t const LED0 = 0b00000011; //PORTC
static uint8_t const LED1 = 0b00011000; //PORTC
static uint8_t const LED2 = 0b11000000; //PORTD
static uint8_t const LED3 = 0b00110000; //PORTB

static uint8_t const PWM0 = 0b00000010; //PORTB
static uint8_t const PWM1 = 0b00000100; //PORTB
static uint8_t const PWM2 = 0b00001000; //PORTB
static uint8_t const PWM3 = 0b00001000; //PORTD

#define OUT(port, led, level) (port ^= (port ^ level) & (led));

void loadRegisters(void) {
	read_register_array(eeprom, registers, REGSIZE * 2);
}
void saveRegisters(void) {
	if(registers[REGSIZE-1] == 1) {
		// the last register is the save flag, reset it
		registers[REGSIZE-1] = 0;
		write_register_array(eeprom, registers, REGSIZE * 2);
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
	// pull-up on D5 (button input ch2)
	 PORTD = BTN2;

	// UART clock on timer0
	TCCR0B = _BV(CS01); // prescaler 8
	TIMSK0 = _BV(TOIE0); // enable overflow interrupt

	// 8-bit Fast PWM on timer1 (on B1,B2) (inverted)
	TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10) | _BV(WGM12);
	TCCR1B = _BV(CS10) | _BV(WGM12); // 31.25 KHz (prescaler 1)

	// 8-bit Fast PWM on timer2 (on B3,D3) (inverted)
	TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20) | _BV(WGM21);
	TCCR2B = _BV(CS20); // 31.25 KHz (prescaler 1)
}

ISR(TIMER0_OVF_vect) { 
	modbusTickTimer();
}

void setLevel(uint8_t channel, uint8_t level) {
	registers[channel] = level;
	switch(channel) {
		case 0:
			OCR1A = registers[4 + level];
			OUT(PORTC, LED0, registers[0]);
		break;
		case 1:
			OCR1B = registers[8 + level];
			OUT(PORTC, LED1, registers[1]<<3);
		break;
		case 2:
			OCR2A = registers[12 + level];
			cli(); // protect PORTD writes from ISR
			OUT(PORTD, LED2, registers[2]<<6);
			sei();
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
	uint8_t new_level = (registers[channel] + 1) % 4;
	setLevel(channel, new_level);
}

void modbusGet(void) {
	if (modbusGetBusState() & (1<<ReceiveCompleted)) {
		switch(rxbuffer[1]) {
			case fcReadHoldingRegisters:
				modbusExchangeRegisters(registers,0,REGSIZE);
			break;
			case fcPresetSingleRegister: // fall through
			case fcPresetMultipleRegisters:
				modbusExchangeRegisters(registers,0,REGSIZE);
				updateLevels();
				saveRegisters();
			break;
			default:
				modbusSendException(ecIllegalFunction);
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

	// TODO: enforce limits on register values
	// TODO: make PWM phase configurable
	// TODO: make PWM frequency configurable
	// TODO: make slave address configurable
	// TODO: make comm parameters configurable
	// TODO: button debouncing (ideally non-blocking)
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