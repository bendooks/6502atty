#!/bin/make

PRG=firmware
MCU=atmega1284p
OBJ=avr-firmware.o

BUILD_DATE=$(shell date)
VERSION=1.3

CC		= avr-gcc
LD		= avr-ld
OBJCOPY		= avr-objcopy
OBJDUMP		= avr-objdump

# atpack for atmega devices downloader from "Atmel"
ATPACK		= $(shell pwd)/atpack/atmega/

# using the external crystal option at 18.432MHz, used 1:1 as we're at 5V
XTAL=18432000
FREQ=18432000
#FREQ=9216000
#XTAL=16000000
#FREQ=8000000

# default using RC oscillator as 8MHz
#XTAL=0
#FREQ=8000000
#FREQ=1000000

CFLAGS=-O2 -Wall -DF_CPU=$(FREQ) -DF_XTAL=$(XTAL) -mmcu=$(MCU)
CFLAGS+=-DBUILD_DATE="\"$(BUILD_DATE)\""
CFLAGS+=-DVERSION="\"$(VERSION)\""

# include support from the atmel device pack
CFLAGS+=-I$(ATPACK)/include/ -B$(ATPACK)/gcc/dev/atmega324pb/
LDFLAGS= -Wl,-Map,$(PRG).map

# set the version of the hardware
#CFLAGS+=-DV2R1=1
CFLAGS+=-DV2R2=1

# note, the 324pb requeest hfuse to be 0xD9 to work

# http://www.engbedded.com/fusecalc/
# set Int RC-OSC, CLKDIV8=1, BOR=2.7V
#AVRFUSE=-U lfuse:w:0x42:m -U hfuse:w:0x99:m -U efuse:w:0xfd:m


# int rc
#AVRFUSE=U lfuse:w:0x42:m -U hfuse:w:0x99:m -U efuse:w:0xff:m

# ext crystal
AVRFUSE=-U lfuse:w:0x77:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m


all:	git-rev build-info $(PRG).elf lst text eeprom

.PHONY: report
report:
	readelf -S firmware.elf | gawk -f report.awk


text: hex bin srec
hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

avr-firmware.o:	pin_defaults.h git-rev.h gpio.h firmware.h  Makefile


pin_defaults.h:	pins.h extract_defaults.awk Makefile
	$(CC) -fdirectives-only -E $(CFLAGS) pins.h | sed -e 's/'\('/ /g' | sed -e  's/'\)'/ /g' | awk -f extract_defaults.awk > pin_defaults.h

.PHONY: git-rev
git-rev:
	@sh git-rev.sh

.PHONY: build-info
build-info:
	@sh build-info.sh


.PHONY: prog
prog:
	avrdude -C +./avrdude.conf -c usbasp -v -p m1284p -F  $(AVRFUSE) -U  $(PRG).hex

.PHONY: clean
clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# Rules for building the .eeprom rom images
eeprom: ehex ebin esrec
ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec

%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ \
	|| { echo empty $@ not generated; exit 0; }

%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ \
	|| { echo empty $@ not generated; exit 0; }
