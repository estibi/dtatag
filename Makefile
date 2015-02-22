ARMCC=/opt/armtc/usr/bin/arm-pc-solaris2.11-gcc
ARMAS=/opt/armtc/usr/gnu/bin/gas
ARMLD=/opt/armtc/usr/bin/ld
ARMOBJCOPY=/opt/armtc/usr/gnu/bin/gobjcopy

CFLAGS=-Wall -O2 -g -DDEBUG -Ilibfdt
ARMCFLAGS=-ffreestanding -DTGT
VERDEF=-DVERSION='"$(shell git describe --tags --dirty)"'

all: dtatag

clean:
	rm -f dtatag

dtatag: main.c start.s bcm2835_uart.c
	$(ARMCC) -x assembler-with-cpp -c -o start.o start.s
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o main.o main.c
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o bcm2835_uart.o bcm2835_uart.c
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o tgt_support.o tgt_support.c
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o atag.o atag.c
	$(ARMLD) -dy -b -znointerp -o dtatag.elf -e _start -M mapfile \
		start.o \
		main.o \
		bcm2835_uart.o \
		tgt_support.o \
		atag.o \
		libfdt/libfdt.a
	$(ARMOBJCOPY) dtatag.elf -O binary $@
