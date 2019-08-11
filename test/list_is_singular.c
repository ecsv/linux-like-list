// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2014, Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>

#include "common.h"

int main(void)
{
	struct list_head testlist;
	struct listitem item1;
	struct listitem item2;

	INIT_LIST_HEAD(&testlist);
	assert(list_empty(&testlist));
	assert(!list_is_singular(&testlist));

	list_add(&item1.list, &testlist);
	assert(!list_empty(&testlist));
	assert(list_is_singular(&testlist));

	list_add(&item2.list, &testlist);
	assert(!list_empty(&testlist));
	assert(!list_is_singular(&testlist));

	return 0;
}
