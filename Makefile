ARMCC=/opt/armtc/usr/bin/arm-pc-solaris2.11-gcc
ARMAS=/opt/armtc/usr/gnu/bin/gas
ARMLD=/opt/armtc/usr/bin/ld
ARMOBJCOPY=/opt/armtc/usr/gnu/bin/gobjcopy

CFLAGS=-Wall -O2 -g -DDEBUG -Ilibfdt
ARMCFLAGS=-ffreestanding -DTGT
VERDEF=-DVERSION='"$(shell git describe --tags --dirty)"'

LIBS=libuart/libuart-bcm2836.a \
     libfdt/libfdt.a

all: dtatag

clean:
	rm -f dtatag

libuart/libuart-bcm2836.a:
	cd libuart; CC=$(ARMCC) gmake

dtatag: main.c start.s tgt_support.c atag.c $(LIBS)
	$(ARMCC) -x assembler-with-cpp -c -o start.o start.s
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o main.o main.c
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o tgt_support.o tgt_support.c
	$(ARMCC) $(CFLAGS) $(ARMCFLAGS) $(VERDEF) -c -o atag.o atag.c
	$(ARMLD) -dy -b -znointerp -o dtatag.elf -e _start -M mapfile \
		start.o \
		main.o \
		tgt_support.o \
		atag.o \
		$(LIBS)
	$(ARMOBJCOPY) dtatag.elf -O binary $@
