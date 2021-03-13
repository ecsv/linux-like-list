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
	struct hlist_head testlist;
	struct hlistitem item1;
	struct hlistitem item2;
	struct hlistitem item3;
	struct hlistitem item4;
	struct hlistitem *item;
	size_t i;

	item1.i = 1;
	item2.i = 2;
	item3.i = 3;
	item4.i = 4;

	INIT_HLIST_HEAD(&testlist);
	assert(hlist_empty(&testlist));

	hlist_add_head(&item4.list, &testlist);
	hlist_add_before(&item2.list, &item4.list);
	hlist_add_before(&item1.list, &item2.list);
	hlist_add_before(&item3.list, &item4.list);

	i = 1;
	hlist_for_each_entry_t(item, &testlist, struct hlistitem, list) {
		assert(item->i == i);
		i++;
	}
	assert(i == 5);

	return 0;
}
