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
	struct listitem item3;
	struct listitem item4;
	struct listitem *item;
	size_t i;

	item1.i = 1;
	item2.i = 2;
	item3.i = 3;
	item4.i = 4;

	INIT_LIST_HEAD(&testlist);
	assert(list_empty(&testlist));

	list_add(&item4.list, &testlist);
	list_add_before(&item2.list, &item4.list);
	list_add_before(&item1.list, &item2.list);
	list_add_before(&item3.list, &item4.list);

	i = 1;
	list_for_each_entry_t(item, &testlist, struct listitem, list) {
		assert(item->i == i);
		i++;
	}
	assert(i == 5);

	return 0;
}
