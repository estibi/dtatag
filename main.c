#include <inttypes.h>

#include "bcm2835_uart.h"
#include "tgt_support.h"

void puts(const char *str)
{
	const char *c = str;

	while (*c != '\0')
		bcm2835_uart_putc(*c++);
}

static void
puthex(uint32_t v)
{
	char buf[2 + 8 + 1];
	char c;
	int i;

	buf[0] = '0';
	buf[1] = 'x';
	buf[10] = '\0';

	for (i = 7; i >= 0; i--) {
		c = v & 0xf;
		v >>= 4;

		if (c > 9)
			c += 'a' - 10;
		else
			c += '0';

		buf[2 + i] = c;
	}

	puts(buf);
}

void write(void *buf, uint32_t len)
{
	uint8_t *ptr = buf;

	while (len) {
		bcm2835_uart_putbyte(*ptr);

		ptr++;
		len--;
	}
}

static void dump_dt(void *dtb)
{
	puts("DeviceTree detected\n");
}

static void dump_atag(void *atags)
{
	puts("ATAG detected\n");
}

void main(uint32_t r0, uint32_t r1, uint32_t r2)
{
	uint32_t *data = (uint32_t *)(uintptr_t)r2;
	extern uint8_t _edata;
	extern uint8_t _end;

	memset(&_edata, 0, &_end - &_edata);

	bcm2835_uart_init();

	puts("Welcome to DT-ATAG " VERSION "...\n");
	puts("r2 = ");
	puthex(r2);
	puts("\n");

	if (data[0] == 0xedfe0dd0 /* 0xd00dfeed */)
		dump_dt(data);
	else if (data[1] == 0x54410001)
		dump_atag(data);
	else
		puts("Unknown boot mode\n");

	puts("Spinning forever!\n");
	for (;;)
		;
}
