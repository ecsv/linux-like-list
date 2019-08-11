// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: 2012-2017, Sven Eckelmann <sven@narfation.org>
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
	struct hpqueue_node *parent;
	uint16_t degree;
	uint8_t mark;
};

struct hpqueue {
	struct hlist_head trees;
	struct hpqueue_node *min;
};

#define HPQUEUE_MAX_DEGREE (2 * sizeof(struct hpqueue_node *) * 8 + 1)

static __inline__ void INIT_HPQUEUE_ROOT(struct hpqueue *queue)
{
	queue->min = NULL;
	INIT_HLIST_HEAD(&queue->trees);
}

static void hpqueue_merge_roots(struct hpqueue *queue,
				int (*compar)(const struct hpqueue_node *,
					      const struct hpqueue_node *))
{
	struct hpqueue_node *degree_map[HPQUEUE_MAX_DEGREE] = { NULL };
	struct hpqueue_node *c;
	struct hlist_node *s;
	uint16_t degree;

	hlist_for_each_entry_safe_t(c, s, &queue->trees, struct hpqueue_node,
				    list) {
		hlist_del(&c->list);
		c->parent = NULL;
		c->mark = 0;

		degree = c->degree;
		while (degree_map[degree]) {
			if (compar(c, degree_map[degree]) < 0) {
				hlist_add_head(&degree_map[degree]->list,
					       &c->children);
				c->degree = degree + 1;
				degree_map[degree]->parent = c;
			} else {
				hlist_add_head(&c->list,
					       &degree_map[degree]->children);
				degree_map[degree]->degree = degree + 1;
				c->parent = degree_map[degree];
				c = degree_map[degree];
			}

			degree_map[degree] = NULL;
			degree++;
		}

		degree_map[degree] = c;
	}

	for (degree = 0; degree < HPQUEUE_MAX_DEGREE; degree++) {
		if (!degree_map[degree])
			continue;

		hlist_add_head(&degree_map[degree]->list, &queue->trees);
	}

}

static void hpqueue_cut_marked(struct hpqueue *queue, struct hpqueue_node *parent)
{
	struct hpqueue_node *c;

	while (parent->mark && parent->parent) {
		c = parent;
		parent = c->parent;

		c->mark = 0;
		hlist_del(&c->list);
		hlist_add_head(&c->list, &queue->trees);
		c->parent = NULL;
		c->mark = 0;

		parent->degree--;
	}

	if (parent)
		parent->mark = 1;
}

static void hpqueue_remove(struct hpqueue *queue, struct hpqueue_node *n,
			   int (*compar)(const struct hpqueue_node *,
					 const struct hpqueue_node *))
{
	struct hpqueue_node *new_min;
	struct hpqueue_node *c;
	struct hlist_node *s;

	hlist_del(&n->list);

	hlist_for_each_entry_safe_t(c, s, &n->children, struct hpqueue_node,
				    list) {
		hlist_del(&c->list);
		hlist_add_head(&c->list, &queue->trees);
		c->parent = NULL;
		c->mark = 0;
	}

	if (n->parent) {
		n->parent->degree--;
		hpqueue_cut_marked(queue, n->parent);
	}
	hpqueue_merge_roots(queue, compar);

	/* update queue root when removed element was minimum */
	if (queue->min == n) {
		new_min = NULL;
		hlist_for_each_entry_t(c, &queue->trees, struct hpqueue_node,
				       list) {
			if (!new_min) {
				new_min = c;
			} else {
				if (compar(c, new_min) < 0)
					new_min = c;
			}
		}

		queue->min = new_min;
	}
}

static void hpqueue_insert(struct hpqueue *queue, struct hpqueue_node *n,
			   int (*compar)(const struct hpqueue_node *,
					 const struct hpqueue_node *))
{
	INIT_HLIST_HEAD(&n->children);
	n->parent = NULL;
	n->degree = 0;
	n->mark = 0;

	hlist_add_head(&n->list, &queue->trees);

	if (!queue->min) {
		queue->min = n;
	} else {
		if (compar(n, queue->min) < 0)
			queue->min = n;
	}
}

static struct hpqueue_node *
hpqueue_pop(struct hpqueue *queue,
	    int (*compar)(const struct hpqueue_node *,
			  const struct hpqueue_node *))
{
	struct hpqueue_node *min = queue->min;

	if (min)
		hpqueue_remove(queue, queue->min, compar);

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
	struct hpqueue queue;
	size_t i;
	struct pqueueitem *item;
	struct hpqueue_node *n;
	uint16_t operation;

	for (i = 0; i < 256; i++) {
		random_shuffle_array(values, (uint16_t)ARRAY_SIZE(values));
		inserted = 0;
		queuelen = 0;

		INIT_HPQUEUE_ROOT(&queue);
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
		assert(queue.min == NULL);
	}

	return 0;
}
