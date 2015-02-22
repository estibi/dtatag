#include <stdint.h>

#include "tgt_support.h"

void memcpy(void *dst, const void *src, uint32_t len)
{
	const uint8_t *s = src;
	uint8_t *d = dst;

	while (len) {
		*d = *s;

		d++;
		s++;
		len--;
	}
}

void memset(void *dst, int val, uint32_t len)
{
	uint8_t *ptr = dst;

	while (len) {
		*ptr = val;

		ptr++;
		len--;
	}
}

char *strstr(const char *as1, const char *as2)
{
	const char *s1, *s2;
	const char *tptr;
	char c;

	s1 = as1;
	s2 = as2;

	if (s2 == NULL || *s2 == '\0')
		return ((char *)s1);

	c = *s2;
	while (*s1 != '\0') {
		if (c == *s1++) {
			tptr = s1;
			while ((c = *++s2) == *s1++ && c != '\0')
				continue;
			if (c == '\0')
				return ((char *)tptr - 1);
			s1 = tptr;
			s2 = as2;
			c = *s2;
		}
	}

	return (NULL);
}

void *memchr(const void *sptr, int c1, size_t n)
{
	if (n != 0) {
		unsigned char c = (unsigned char)c1;
		const unsigned char *sp = sptr;

		do {
			if (*sp++ == c)
				return ((void *)--sp);
		} while (--n != 0);
	}
	return (NULL);
}

void memmove(void *s, const void *s0, size_t n)
{
	if (n != 0) {
		char *s1 = s;
		const char *s2 = s0;

		if (s1 <= s2) {
			do {
				*s1++ = *s2++;
			} while (--n != 0);
		} else {
			s2 += n;
			s1 += n;
			do {
				*--s1 = *--s2;
			} while (--n != 0);
		}
	}
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	if (s1 != s2 && n != 0) {
		const unsigned char *ps1 = s1;
		const unsigned char *ps2 = s2;

		do {
			if (*ps1++ != *ps2++)
				return (ps1[-1] - ps2[-1]);
		} while (--n != 0);
	}

	return (0);
}

char * strchr(const char *sp, int c)
{
	do {
		if (*sp == (char)c)
			return ((char *)sp);
	} while (*sp++);
	return (NULL);
}

size_t strlen(const char *s)
{
	const char *s0 = s + 1;

	while (*s++ != '\0')
		;
	return (s - s0);
}
