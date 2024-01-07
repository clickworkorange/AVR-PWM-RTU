
#ifndef AVR_PWM_RTU_H
#define AVR_PWM_RTU_H
#define REVISION 1001
#define CHANNELS 4
#define LEVELS 4
#define REGCOUNT 51

// size in bits of max register value - set to zero for read only registers
static  uint8_t const REGSIZE[] = {2,2,2,2,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,1,3,1,3,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,8,3,2,1,0,0,1};
// "factory default" values for each register 
static uint16_t const DEFAULT[] = {0,0,0,0,255,127,63,0,255,127,63,0,255,127,63,0,255,127,63,0,1,4,1,4,20567,19757,12544,0,17256,12288,0,0,17256,12544,0,0,17256,12800,0,0,17256,13056,0,0,1,2,0,1,0,REVISION,1};

// EEPROM helper macros
#define read_register_array(address,value_p,length) eeprom_read_block ((void *)value_p, (const void *)address, length)
#define write_register_array(address,value_p,length) eeprom_write_block ((const void *)value_p, (void *)address, length)

#endif