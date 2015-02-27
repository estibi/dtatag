#include <inttypes.h>

#include "uart.h"
#include "tgt_support.h"
#include "atag.h"
#include <libfdt.h>

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

#define DUMP_DT_NODE_NAME(fdt, node)					\
	do {								\
		char tmp[128];						\
		fdt_get_path((fdt), (node), tmp, sizeof(tmp));		\
		puts(tmp);						\
		puts("\n");						\
	} while (0)

static void hexdump(const uint8_t *buf, int len)
{
	static const char *hex[16] = {
		"0", "1", "2", "3", "4", "5", "6", "7",
		"8", "9", "a", "b", "c", "d", "e", "f",
	};

	while (len) {
		puts(hex[*buf >> 4]);
		puts(hex[*buf & 0xf]);
		puts(" ");
		buf++;
		len--;
	}
}

static void dump_dt_props(const void *fdt, int node)
{
	int ret;

	ret = fdt_first_property_offset(fdt, node);
	if (ret < 0)
		return;

	for (;;) {
		const void *val;
		const char *name;
		int len;

		val = fdt_getprop_by_offset(fdt, ret, &name, &len);
		puts("\t");
		puts(name);
		puts(" (");
		puthex(len);
		puts("): ");
		hexdump(val, len);
		puts("\n");

		ret = fdt_next_property_offset(fdt, ret);
		if (ret < 0)
			return;
	}
}

static void dump_dt_node(void *fdt, int node)
{
	DUMP_DT_NODE_NAME(fdt, node);
	dump_dt_props(fdt, node);

	node = fdt_first_subnode(fdt, node);
	if (node < 0)
		return;

	for (;;) {
		dump_dt_node(fdt, node);

		node = fdt_next_subnode(fdt, node);
		if (node < 0)
			return;
	}
}

static void dump_dt(void *fdt)
{
	puts("DeviceTree detected\n");

	dump_dt_node(fdt, fdt_path_offset(fdt, "/"));
}

#define DUMP_ATAG_VAL(name, val)					\
	do {								\
		puts("\t" name ":\n");					\
		puts("\t");						\
		puthex(val);						\
		puts("\n");						\
	} while (0)

static void dump_atag(atag_header_t *h)
{
	atag_core_t *acp;
	atag_mem_t *amp;
	atag_cmdline_t *alp;
	atag_initrd_t *aip;
	atag_illumos_status_t *aisp;
	atag_illumos_mapping_t *aimp;
	const char *tname;

	puts("ATAG detected\n");

	while (h != NULL) {
		switch (h->ah_tag) {
		case ATAG_CORE:
			tname = "ATAG_CORE";
			break;
		case ATAG_MEM:
			tname = "ATAG_MEM";
			break;
		case ATAG_VIDEOTEXT:
			tname = "ATAG_VIDEOTEXT";
			break;
		case ATAG_RAMDISK:
			tname = "ATAG_RAMDISK";
			break;
		case ATAG_INITRD2:
			tname = "ATAG_INITRD2";
			break;
		case ATAG_SERIAL:
			tname = "ATAG_SERIAL";
			break;
		case ATAG_REVISION:
			tname = "ATAG_REVISION";
			break;
		case ATAG_VIDEOLFB:
			tname = "ATAG_VIDEOLFB";
			break;
		case ATAG_CMDLINE:
			tname = "ATAG_CMDLINE";
			break;
		case ATAG_ILLUMOS_STATUS:
			tname = "ATAG_ILLUMOS_STATUS";
			break;
		case ATAG_ILLUMOS_MAPPING:
			tname = "ATAG_ILLUMOS_MAPPING";
			break;
		default:
			tname = "???";
			break;
		}

		puts("tag: ");
		puts(tname);
		puts("(");
		puthex(h->ah_size);
		puts(")\n");

		/* Extended information */
		switch (h->ah_tag) {
		case ATAG_CORE:
			if (h->ah_size == 2) {
				puts("ATAG_CORE has no extra information\n");
			} else {
				acp = (atag_core_t *)h;
				DUMP_ATAG_VAL("flags", acp->ac_flags);
				DUMP_ATAG_VAL("pagesize", acp->ac_pagesize);
				DUMP_ATAG_VAL("rootdev", acp->ac_rootdev);
			}
			break;
		case ATAG_MEM:
			amp = (atag_mem_t *)h;
			DUMP_ATAG_VAL("size", amp->am_size);
			DUMP_ATAG_VAL("start", amp->am_start);
			break;
		case ATAG_INITRD2:
			aip = (atag_initrd_t *)h;
			DUMP_ATAG_VAL("size", aip->ai_size);
			DUMP_ATAG_VAL("start", aip->ai_start);
			break;
		case ATAG_CMDLINE:
			alp = (atag_cmdline_t *)h;
			puts("\tcmdline:");
			puts(alp->al_cmdline);
			puts("\n");
			break;
		case ATAG_ILLUMOS_STATUS:
			aisp = (atag_illumos_status_t *)h;
			DUMP_ATAG_VAL("version", aisp->ais_version);
			DUMP_ATAG_VAL("ptbase", aisp->ais_ptbase);
			DUMP_ATAG_VAL("freemem", aisp->ais_freemem);
			DUMP_ATAG_VAL("freeused", aisp->ais_freeused);
			DUMP_ATAG_VAL("archive", aisp->ais_archive);
			DUMP_ATAG_VAL("archivelen", aisp->ais_archivelen);
			DUMP_ATAG_VAL("pt_arena", aisp->ais_pt_arena);
			DUMP_ATAG_VAL("pt_arena_max", aisp->ais_pt_arena_max);
			DUMP_ATAG_VAL("stext", aisp->ais_stext);
			DUMP_ATAG_VAL("etext", aisp->ais_etext);
			DUMP_ATAG_VAL("sdata", aisp->ais_sdata);
			DUMP_ATAG_VAL("edata", aisp->ais_edata);
			break;
		case ATAG_ILLUMOS_MAPPING:
			aimp = (atag_illumos_mapping_t *)h;
			DUMP_ATAG_VAL("paddr", aimp->aim_paddr);
			DUMP_ATAG_VAL("plen", aimp->aim_plen);
			DUMP_ATAG_VAL("vaddr", aimp->aim_vaddr);
			DUMP_ATAG_VAL("vlen", aimp->aim_vlen);
			DUMP_ATAG_VAL("mapflags", aimp->aim_mapflags);
			break;
		default:
			break;
		}

		h = atag_next(h);
	}
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
		dump_atag((atag_header_t *)data);
	else
		puts("Unknown boot mode\n");

	puts("Spinning forever!\n");
	for (;;)
		;
}
