// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2014, Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>
#include <stdlib.h>

#include "common.h"

static uint16_t values[256];

static void list_insert_sorted(struct hlistitem *entry, struct hlist_head *head)
{
	struct hlistitem *item;
	struct hlistitem *last = NULL;

	hlist_for_each_entry_t(item, head, struct hlistitem, list) {
		if (cmpint(&entry->i, &item->i) < 0) {
			hlist_add_before(&entry->list, &item->list);
			return;
		}
		last = item;
	}

	if (!last)
		hlist_add_head(&entry->list, head);
	else
		hlist_add_behind(&entry->list, &last->list);
}

static void list_insertsort(struct hlist_head *head)
{
	struct hlist_head list_unsorted;
	struct hlistitem *item;
	struct hlist_node *is;

	hlist_move_list(head, &list_unsorted);

	hlist_for_each_entry_safe_t(item, is, &list_unsorted, struct hlistitem,
				    list) {
		hlist_del(&item->list);
		list_insert_sorted(item, head);
	}
}

int main(void)
{
	struct hlist_head testlist;
	struct hlistitem *item;
	struct hlist_node *is;
	size_t i;

	random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));

	INIT_HLIST_HEAD(&testlist);

	assert(hlist_empty(&testlist));

	for (i = 0; i < ARRAY_SIZE(values); i++) {
		item = (struct hlistitem *)malloc(sizeof(*item));
		assert(item);
		item->i = values[i];
		hlist_add_head(&item->list, &testlist);
	}

	assert(!hlist_empty(&testlist));

	qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
	list_insertsort(&testlist);

	i = 0;
	hlist_for_each_entry_safe_t(item, is, &testlist, struct hlistitem,
				    list) {
		assert(item->i == values[i]);
		hlist_del(&item->list);
		free(item);
		i++;
	}

	assert(i == ARRAY_SIZE(values));
	assert(hlist_empty(&testlist));

	return 0;
}
