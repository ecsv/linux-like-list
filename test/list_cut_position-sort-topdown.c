// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>
#include <stdlib.h>

#include "common.h"

static uint16_t values[256];

static size_t list_cut_elements(struct list_head *head_to,
				struct list_head *head_from,
				size_t elements)
{
	struct list_head *item;
	struct list_head *middle;

	INIT_LIST_HEAD(head_to);

	middle = head_from;
	list_for_each(item, head_from) {
		elements--;
		middle = item;

		if (!elements)
			break;
	}

	list_cut_position(head_to, head_from, middle);

	return elements;
}

static void list_merge_ordered(struct list_head *list1, struct list_head *list2,
			       struct list_head *head)
{
	struct listitem *item1, *item2;

	while (!list_empty(list1) && !list_empty(list2)) {
		item1 = list_first_entry(list1, struct listitem, list);
		item2 = list_first_entry(list2, struct listitem, list);

		if (cmpint(&item1->i, &item2->i) <= 0)
			list_move_tail(&item1->list, head);
		else
			list_move_tail(&item2->list, head);
	}

	list_splice_tail(list1, head);
	list_splice_tail(list2, head);
}

static void list_mergesort(struct list_head *head, size_t elements)
{
	struct list_head list1, list2;
	size_t elements_half = elements / 2;
	size_t missing;

	if (list_empty(head) || list_is_singular(head))
		return;

	INIT_LIST_HEAD(&list1);
	INIT_LIST_HEAD(&list2);

	missing = list_cut_elements(&list1, head, elements_half);
	assert(!missing);

	missing = list_cut_elements(&list2, head, elements - elements_half);
	assert(!missing);

	assert(list_empty(head));

	list_mergesort(&list1, elements_half);
	list_mergesort(&list2, elements - elements_half);

	list_merge_ordered(&list1, &list2, head);
}

int main(void)
{
	struct list_head testlist;
	struct listitem *item, *is;
	size_t i;

	random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));

	INIT_LIST_HEAD(&testlist);

	assert(list_empty(&testlist));

	for (i = 0; i < ARRAY_SIZE(values); i++) {
		item = (struct listitem *)malloc(sizeof(*item));
		assert(item);
		item->i = values[i];
		list_add_tail(&item->list, &testlist);
	}

	assert(!list_empty(&testlist));

	qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
	list_mergesort(&testlist, ARRAY_SIZE(values));

	i = 0;
	list_for_each_entry_safe_t(item, is, &testlist, struct listitem, list) {
		assert(item->i == values[i]);
		list_del(&item->list);
		free(item);
		i++;
	}

	assert(i == ARRAY_SIZE(values));
	assert(list_empty(&testlist));

	return 0;
}
