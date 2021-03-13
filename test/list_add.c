// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>

#include "common.h"

int main(void)
{
	struct list_head testlist;
	struct listitem item;

	INIT_LIST_HEAD(&testlist);
	assert(list_empty(&testlist));

	list_add(&item.list, &testlist);
	assert(!list_empty(&testlist));

	return 0;
}
