
#ifndef AVR_PWM_RTU_H
#define AVR_PWM_RTU_H

// why is BAUD definition here ignored by #ifndef in yaMBSiavr.h?
// #define BAUD 9600L

#define CHANNELS 4
#define REGSIZE 51
#define read_register_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
#define write_register_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)

#endif