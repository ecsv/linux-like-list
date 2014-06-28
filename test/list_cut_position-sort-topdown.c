/* Minimal Linux-like double-linked list helper functions test
 *
 * Copyright (c) 2012-2014, Sven Eckelmann <sven@narfation.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "../list.h"
#include <assert.h>
#include <stdlib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static int values[] = {
	166, 110, 41, 144, 130, 131, 209, 193, 8, 83, 5, 97, 231, 141, 212, 117,
	243, 11, 150, 104, 69, 111, 18, 232, 55, 0, 198, 114, 31, 98, 68, 156,
	184, 100, 199, 128, 65, 70, 204, 195, 250, 226, 153, 73, 140, 87, 196,
	85, 56, 172, 177, 19, 82, 152, 201, 174, 221, 84, 30, 222, 170, 148,
	116, 50, 93, 10, 200, 247, 71, 49, 134, 248, 249, 175, 29, 143, 51, 237,
	109, 217, 136, 79, 245, 127, 113, 54, 171, 26, 94, 12, 62, 223, 203, 74,
	115, 213, 35, 197, 176, 192, 142, 123, 163, 59, 233, 72, 121, 219, 146,
	246, 27, 21, 188, 162, 14, 15, 179, 7, 96, 255, 63, 183, 205, 120, 75,
	102, 160, 161, 124, 13, 190, 194, 80, 37, 251, 45, 112, 242, 129, 178,
	3, 33, 165, 86, 57, 39, 252, 244, 202, 239, 234, 225, 67, 137, 34, 107,
	133, 206, 46, 238, 241, 145, 61, 155, 253, 103, 108, 22, 78, 101, 208,
	135, 181, 42, 118, 236, 52, 106, 38, 6, 138, 191, 77, 216, 182, 32, 126,
	122, 125, 186, 240, 95, 211, 4, 48, 91, 20, 180, 1, 207, 157, 16, 66,
	254, 210, 218, 147, 25, 44, 9, 47, 235, 99, 169, 40, 167, 2, 132, 60,
	189, 23, 88, 24, 105, 36, 28, 119, 185, 159, 168, 224, 229, 228, 53,
	151, 89, 173, 154, 149, 92, 164, 215, 43, 158, 214, 58, 230, 81, 17, 64,
	220, 90, 76, 227, 187, 139
};

struct listitem {
	int i;
	struct list_head list;
};

static int cmpint(const void *p1, const void *p2)
{
	const int *i1 = (const int *)p1;
	const int *i2 = (const int *)p2;

	return *i1 - *i2;
}

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

		if (cmpint(&item1->i, &item2->i) < 0)
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
	if (missing) {
		elements_half -= - missing;
		elements = elements_half;
	}

	missing = list_cut_elements(&list2, head, elements - elements_half);
	if (missing)
		elements -= missing;

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
