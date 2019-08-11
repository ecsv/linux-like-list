// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2014, Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>

int main(void)
{
	struct list_head testlist;

	INIT_LIST_HEAD(&testlist);
	assert(list_empty(&testlist));

	return 0;
}
