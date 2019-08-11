// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2014, Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>
#include <stdlib.h>

#include "common.h"

int main(void)
{
	struct list_head testlist, testlist2;
	struct listitem *item, *is;
	size_t i;

	INIT_LIST_HEAD(&testlist);
	INIT_LIST_HEAD(&testlist2);

	assert(list_empty(&testlist));
	assert(list_empty(&testlist2));

	for (i = 5; i < 10; i++) {
		item = (struct listitem *)malloc(sizeof(*item));
		assert(item);
		item->i = i;
		list_add_tail(&item->list, &testlist);
	}

	assert(!list_empty(&testlist));

	list_splice_init(&testlist2, &testlist);
	assert(list_empty(&testlist2));

	for (i = 0; i < 5; i++) {
		item = (struct listitem *)malloc(sizeof(*item));
		assert(item);
		item->i = i;
		list_add_tail(&item->list, &testlist2);
	}

	assert(!list_empty(&testlist2));

	list_splice_init(&testlist2, &testlist);
	assert(list_empty(&testlist2));

	i = 0;
	list_for_each_entry_t(item, &testlist, struct listitem, list) {
		assert(item->i == i);
		i++;
	}

	assert(i == 10);

	list_splice_init(&testlist, &testlist2);
	assert(!list_empty(&testlist2));
	assert(list_empty(&testlist));

	i = 0;
	list_for_each_entry_safe_t(item, is, &testlist2, struct listitem,
				   list) {
		assert(item->i == i);
		list_del(&item->list);
		free(item);
		i++;
	}

	assert(i == 10);
	assert(list_empty(&testlist2));

	return 0;
}
