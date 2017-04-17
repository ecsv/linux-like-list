/* Minimal Linux-like double-linked list helper functions test
 *
 * Copyright (c) 2012-2017, Sven Eckelmann <sven@narfation.org>
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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../list.h"
#include "common.h"

static uint16_t values[256];
static uint16_t inserted;

static uint16_t valuequeue[ARRAY_SIZE(values)];
static uint16_t queuelen;

static uint16_t valuequeue_getmin(void)
{
	size_t i;
	uint16_t t;
	uint16_t *min_pos = NULL;

	for (i = 0; i < queuelen; i++) {
		if (!min_pos || valuequeue[i] < *min_pos)
			min_pos = &valuequeue[i];
	}

	if (!min_pos)
		return 0;

	t = valuequeue[queuelen - 1];
	valuequeue[queuelen - 1] = *min_pos;
	*min_pos = t;

	return valuequeue[queuelen - 1];
}

struct hpqueue_node {
	struct hlist_node list;
	struct hlist_head children;
};

static struct hpqueue_node *
hpqueue_merge(struct hpqueue_node *p1, struct hpqueue_node *p2,
	      int (*compar)(const struct hpqueue_node *,
			    const struct hpqueue_node *))
{
	if (!p1)
		return p2;

	if (!p2)
		return p1;

	if (compar(p1, p2) < 0) {
		hlist_add_head(&p2->list, &p1->children);
		return p1;
	} else {
		hlist_add_head(&p1->list, &p2->children);
		return p2;
	}
}

static struct hpqueue_node *
hpqueue_mergepairs(struct hlist_head *children,
		   int (*compar)(const struct hpqueue_node *,
				 const struct hpqueue_node *))
{
	struct hpqueue_node *p1;
	struct hpqueue_node *p2;
	struct hpqueue_node *r;
	struct hlist_head ltr_pairs;
	struct hlist_node *s;

	INIT_HLIST_HEAD(&ltr_pairs);

	/* left to right */
	while (!hlist_empty(children)) {
		p1 = hlist_entry(children->first, struct hpqueue_node, list);
		hlist_del(&p1->list);

		if (hlist_empty(children)) {
			hlist_add_head(&p1->list, &ltr_pairs);
			break;
		}

		p2 = hlist_entry(children->first, struct hpqueue_node, list);
		hlist_del(&p2->list);

		r = hpqueue_merge(p1, p2, compar);
		hlist_add_head(&r->list, &ltr_pairs);
	}

	/* right to left merge */
	r = hlist_entry(ltr_pairs.first, struct hpqueue_node, list);
	hlist_del(&r->list);

	hlist_for_each_entry_safe_t(p2, s, &ltr_pairs, struct hpqueue_node, list) {
		p1 = r;
		hlist_del(&p2->list);

		r = hpqueue_merge(p1, p2, compar);
	}

	return r;
}

static void hpqueue_remove(struct hpqueue_node **queue,
			   struct hpqueue_node *n,
			   int (*compar)(const struct hpqueue_node *,
					 const struct hpqueue_node *))
{
	struct hpqueue_node *new_min;
	struct hlist_head children;

	/* no children to merge -> pairwise merge */
	if (hlist_empty(&n->children)) {
		hlist_del(&n->list);

		/* update queue root when removed element was root */
		if (*queue == n)
			*queue = NULL;
		return;
	}

	INIT_HLIST_HEAD(&children);
	hlist_move_list(&n->children, &children);
	new_min = hpqueue_mergepairs(&children, compar);

	/* update queue root when removed element was root */
	if (*queue == n) {
		*queue = new_min;
		INIT_HLIST_NODE(&new_min->list);
		return;
	}

	/* replace n with new_min in parent root */
	hlist_add_before(&new_min->list, &n->list);
	hlist_del(&n->list);
}

static void hpqueue_insert(struct hpqueue_node **queue, struct hpqueue_node *n,
			   int (*compar)(const struct hpqueue_node *,
					 const struct hpqueue_node *))
{
	struct hpqueue_node *new_min;

	INIT_HLIST_HEAD(&n->children);
	INIT_HLIST_NODE(&n->list);

	new_min = hpqueue_merge(n, *queue, compar);
	*queue = new_min;
}

static struct hpqueue_node *
hpqueue_pop(struct hpqueue_node **queue,
	    int (*compar)(const struct hpqueue_node *,
			  const struct hpqueue_node *))
{
	struct hpqueue_node *min = *queue;

	if (min)
		hpqueue_remove(queue, *queue, compar);

	return min;
}

struct pqueueitem {
	uint16_t i;
	struct hpqueue_node pq;
};

static int pqueue_cmpint(const struct hpqueue_node *p1,
			 const struct hpqueue_node *p2)
{
	const struct pqueueitem *i1 = hlist_entry(p1, struct pqueueitem, pq);
	const struct pqueueitem *i2 = hlist_entry(p2, struct pqueueitem, pq);

	return cmpint(&i1->i, &i2->i);
}

int main(void)
{
	struct hpqueue_node *queue;
	size_t i;
	struct pqueueitem *item;
	struct hpqueue_node *n;
	uint16_t operation;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));
		inserted = 0;
		queuelen = 0;

		queue = NULL;
		while (inserted < ARRAY_SIZE(values) ||
		       queuelen != 0) {

			if (inserted == ARRAY_SIZE(values))
				operation = 0;
			else
				operation = get_unsigned16() % 2;

			if (operation == 1) {
				item = (struct pqueueitem *)malloc(sizeof(*item));
				assert(item);

				item->i = values[inserted];
				hpqueue_insert(&queue, &item->pq, pqueue_cmpint);

				valuequeue[queuelen] = values[inserted];
				inserted++;
				queuelen++;
			} else {
				n = hpqueue_pop(&queue, pqueue_cmpint);
				item = hlist_entry_safe(n, struct pqueueitem, pq);

				if (queuelen) {
					assert(item);
					assert(item->i == valuequeue_getmin());


					queuelen--;
					free(item);
				} else {
					assert(!item);
				}

			}
		}
		assert(queue == NULL);
	}

	return 0;
}
