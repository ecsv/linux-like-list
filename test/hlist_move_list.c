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
	struct hlist_head testlist, testlist2;
	struct hlistitem *item;
	struct hlist_node *is;
	size_t i;

	INIT_HLIST_HEAD(&testlist);

	assert(hlist_empty(&testlist));

	for (i = 5; i > 0; i--) {
		item = (struct hlistitem *)malloc(sizeof(*item));
		assert(item);
		item->i = i;
		hlist_add_head(&item->list, &testlist);
	}

	assert(!hlist_empty(&testlist));

	hlist_move_list(&testlist, &testlist2);

	assert(hlist_empty(&testlist));
	assert(!hlist_empty(&testlist2));

	i = 1;
	hlist_for_each_entry_safe_t(item, is, &testlist2, struct hlistitem,
				    list) {
		assert(item->i == i);
		hlist_del(&item->list);
		free(item);
		i++;
	}

	assert(i == 6);
	assert(hlist_empty(&testlist2));

	hlist_move_list(&testlist, &testlist2);
	assert(hlist_empty(&testlist));
	assert(hlist_empty(&testlist2));

	return 0;
}
