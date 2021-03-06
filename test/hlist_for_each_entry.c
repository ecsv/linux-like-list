// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>
#include <stdlib.h>

#include "common.h"

int main(void)
{
	struct hlist_head testlist;
	struct hlistitem *item;
	struct hlist_node *lis;
	size_t i;

	INIT_HLIST_HEAD(&testlist);

	assert(hlist_empty(&testlist));

	for (i = 0; i < 5; i++) {
		item = (struct hlistitem *)malloc(sizeof(*item));
		assert(item);
		item->i = i;
		hlist_add_head(&item->list, &testlist);
	}

	assert(!hlist_empty(&testlist));

	i = 4;
	hlist_for_each_entry(item, &testlist, list) {
		assert(item->i == i);
		i--;
	}

	assert(i == ~((size_t)0));
	assert(!hlist_empty(&testlist));

	i = 4;
	hlist_for_each_entry_safe(item, lis, &testlist, list) {
		assert(item->i == i);
		hlist_del(&item->list);
		free(item);
		i--;
	}

	assert(i == ~((size_t)0));
	assert(hlist_empty(&testlist));

	return 0;
}
