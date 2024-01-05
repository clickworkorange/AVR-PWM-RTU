
#ifndef AVR_PWM_RTU_H
#define AVR_PWM_RTU_H

// why is BAUD definition here ignored by #ifndef in yaMBSiavr.h?
// #define BAUD 9600L

#define CHANNELS 4
#define LEVELS 4
#define REGCOUNT 51
// size in bits of max register value - set to zero for read only registers
static uint8_t const REGSIZE[] = {2,2,2,2,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,1,3,1,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,8,3,2,1,0,0,1};
#define read_register_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
#define write_register_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)

#endif