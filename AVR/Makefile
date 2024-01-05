MCU     = atmega328
F_CPU   = 8000000
CC      = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS  = -Wall -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -Iinclude 
TARGET  = avr_pwm_rtu
INCLUDE = include/yaMBSiavr.c
SRCS    = avr_pwm_rtu.c ${INCLUDE}

all: bin hex

bin:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}

hex:
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

elf:${TARGET}.o
	${CC} ${CFLAGS} ${INCLUDE} -o ${TARGET}.elf ${TARGET}.o

debug: CFLAGS += -g -O0
debug: all elf

flash:
	avrdude -p ${MCU} -P usb -c dragon_isp -U flash:w:${TARGET}.hex:i -F

clean:
	rm -f *.bin *.hex *.elf
