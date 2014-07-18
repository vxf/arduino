# This sample Makefile, explains how you can compile plain AVR C file.
#
# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

NO_CORE = Yes

BOARD_TAG = nano328
MCU = atmega328p
F_CPU = 16000000L

ISP_PROG = stk500v1
AVRDUDE_ISP_BAUDRATE = 57600

AVRDUDE_ARD_PROGRAMMER = arduino
AVRDUDE_ARD_BAUDRATE = 57600

include ../Arduino-Makefile/Arduino.mk

# !!! Important. You have to use make ispload to upload when using ISP programmer
