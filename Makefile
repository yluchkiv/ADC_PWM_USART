MCU=atmega328p
F_CPU=16000000L
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I. -Wl,-u,vfprintf -lprintf_flt -lm
TARGET=ADC_PWM_UART
SRCS=ADC_PWM_UART.c

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

flash:
	avrdude -p ${MCU} -c usbtiny -U flash:w:${TARGET}.hex:i -P usb

clean:
	rm -f *.bin *.hex
