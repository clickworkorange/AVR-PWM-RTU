#!/usr/bin/env python3
import minimalmodbus
import time

instrument = minimalmodbus.Instrument("/dev/pts/7", 1)  # port name, slave address (in decimal)
instrument.serial.baudrate = 9600
instrument.serial.timeout = 2

while True:
	register = input("Register:")
	if register:
		register = int(register)
		value = int(input("Value:"))
		instrument.write_register(register, value)
	val = instrument.read_registers(0, 20)  # start, count
	print(val)

	# cmd = input("C/R:")
	# if cmd == "r" or cmd == "R":
	# 	register = input("Register:")
	# 	if register:
	# 		register = int(register)
	# 		value = int(input("Value:"))
	# 		instrument.write_register(register, value)
	# 	val = instrument.read_registers(0, 16)  # start, count
	# 	print(val)
	# elif cmd == "c" or cmd == "C":
	# 	coil =input("Coil:") 
	# 	if coil: 
	# 		coil = int(coil)
	# 		on = int(input("1/0:"))
	# 		instrument.write_bit(coil, on)
	# 	val = instrument.read_bits(0, 8, 1)  # start, count, fc
	# 	print(val)



# set up two connected virtual serial ports with: 
# socat -d -d pty,link=/tmp/vtty1,raw,echo=0 pty,link=/tmp/vtty2,raw,echo=0