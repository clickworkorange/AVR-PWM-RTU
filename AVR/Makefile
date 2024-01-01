MCU=atmega328
F_CPU=8000000
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -Iinclude
TARGET=avr_pwm_rtu
SRCS=avr_pwm_rtu.c include/yaMBSiavr.c

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

flash:
	avrdude -p ${MCU} -P usb -c dragon_isp -U flash:w:${TARGET}.hex:i -F

clean:
	rm -f *.bin *.hex
