/* Minimal red-black-tree helper functions test
 *
 * Copyright (c) 2012-2016, Sven Eckelmann <sven@narfation.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __LINUX_LIKE_LIST_COMMON_H__
#define __LINUX_LIKE_LIST_COMMON_H__

#include <stddef.h>
#include <stdint.h>

#include "../list.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

struct listitem {
	uint16_t i;
	struct list_head list;
};

struct hlistitem {
	uint16_t i;
	struct hlist_node list;
};

static __inline__ uint8_t getnum(void)
{
	static uint16_t s1 = UINT16_C(2);
	static uint16_t s2 = UINT16_C(1);
	static uint16_t s3 = UINT16_C(1);

	s1 *= UINT16_C(171);
	s1 %= UINT16_C(30269);

	s2 *= UINT16_C(172);
	s2 %= UINT16_C(30307);

	s3 *= UINT16_C(170);
	s3 %= UINT16_C(30323);

	return s1 ^ s2 ^ s3;
}

static uint16_t get_unsigned16(void)
{
	uint16_t x = 0;
	size_t i;

	for (i = 0; i < sizeof(x); i++) {
		x <<= 8;
		x |= getnum();
	}

	return x;
}

static __inline__ int cmpint(const void *p1, const void *p2)
{
	const uint16_t *i1 = (const uint16_t *)p1;
	const uint16_t *i2 = (const uint16_t *)p2;

	return *i1 - *i2;
}

static __inline__ void random_shuffle_array(uint16_t *operations, uint16_t len)
{
	uint16_t i;
	uint16_t j;

	for (i = 0; i < len; i++) {
		/* WARNING biased shuffling */
		j = get_unsigned16() % (i + 1);
		operations[i] = operations[j];
		operations[j] = i;
	}
}

#endif /* __LINUX_LIKE_LIST_COMMON_H__ */
