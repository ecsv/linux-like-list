// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2014, Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>

static LIST_HEAD(testlist);

int main(void)
{
	assert(list_empty(&testlist));

	return 0;
}
