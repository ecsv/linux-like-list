/* Minimal Linux-like double-linked list helper functions
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

#ifndef __LINUX_LIKE_LIST_H__
#define __LINUX_LIKE_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#if defined(__GNUC__)
#define  LIST_TYPEOF_USE 1
#endif

#if defined(_MSC_VER)
#define __inline__ __inline
#endif

#ifndef container_of
#ifdef LIST_TYPEOF_USE
#define container_of(ptr, type, member) __extension__ ({ \
	const __typeof__(((type *)0)->member) *__pmember = (ptr); \
	(type *)((char *)__pmember - offsetof(type, member)); })
#else
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#endif
#endif

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

#define LIST_HEAD(head) \
	struct list_head head = { &(head), &(head) }

static __inline__ void INIT_LIST_HEAD(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

static __inline__ void list_add(struct list_head *node,
				struct list_head *head)
{
	struct list_head *next = head->next;

	next->prev = node;
	node->next = next;
	node->prev = head;
	head->next = node;
}

static __inline__ void list_add_tail(struct list_head *node,
				     struct list_head *head)
{
	struct list_head *prev = head->prev;

	prev->next = node;
	node->next = head;
	node->prev = prev;
	head->prev = node;
}

static __inline__ void list_del(struct list_head *node)
{
	struct list_head *next = node->next;
	struct list_head *prev = node->prev;

	next->prev = prev;
	prev->next = next;

#ifdef LIST_POISONING
	node->prev = (struct list_head *)(0x00100100);
	node->next = (struct list_head *)(0x00200200);
#endif
}

static __inline__ void list_del_init(struct list_head *node)
{
	list_del(node);
	INIT_LIST_HEAD(node);
}

static __inline__ int list_empty(const struct list_head *head)
{
	return (head->next == head);
}

#define list_entry(node, type, member) container_of(node, type, member)

#define list_first_entry(head, type, member) \
	list_entry((head)->next, type, member)

#define list_for_each(node, head) \
	for (node = (head)->next; \
	     node != (head); \
	     node = node->next)

#define list_for_each_entry_t(entry, head, type, member) \
	for (entry = list_entry((head)->next, type, member); \
	     &entry->member != (head); \
	     entry = list_entry(entry->member.next, type, member))

#ifdef LIST_TYPEOF_USE
#define list_for_each_entry(entry, head, member) \
	list_for_each_entry_t(entry, head, __typeof__(*entry), member)
#endif

#define list_for_each_safe(node, safe, head) \
	for (node = (head)->next, safe = node->next; \
	     node != (head); \
	     node = safe, safe = node->next)

#define list_for_each_entry_safe_t(entry, safe, head, type, member) \
	for (entry = list_entry((head)->next, type, member), \
	     safe = list_entry(entry->member.next, type, member); \
	     &entry->member != (head); \
	     entry = safe, \
	     safe = list_entry(safe->member.next, type, member))

#ifdef LIST_TYPEOF_USE
#define list_for_each_entry_safe(entry, safe, head, member) \
	list_for_each_entry_safe_t(entry, safe, head, __typeof__(*entry), \
				   member)
#endif

struct hlist_node {
	struct hlist_node *next;
	struct hlist_node **pprev;
};

struct hlist_head {
	struct hlist_node *first;
};

#define HLIST_HEAD(head) \
	struct hlist_head head = { NULL }

static __inline__ void INIT_HLIST_HEAD(struct hlist_head *head)
{
	head->first = NULL;
}

static __inline__ void INIT_HLIST_NODE(struct hlist_node *node)
{
	node->next = NULL;
	node->pprev = NULL;
}

static __inline__ void hlist_add_head(struct hlist_node *node,
				      struct hlist_head *head)
{
	struct hlist_node *first = head->first;

	head->first = node;
	node->next = first;
	node->pprev = &head->first;
	if (first)
		first->pprev = &node->next;
}

static __inline__ void hlist_del(struct hlist_node *node)
{
	struct hlist_node *next = node->next;
	struct hlist_node **pprev = node->pprev;

	if (pprev)
		*pprev = next;

	if (next)
		next->pprev = pprev;

#ifdef LIST_POISONING
	node->pprev = (struct hlist_node **)(0x00100100);
	node->next = (struct hlist_node *)(0x00200200);
#endif
}

static __inline__ void hlist_del_init(struct hlist_node *node)
{
	hlist_del(node);
	INIT_HLIST_NODE(node);
}

static __inline__ int hlist_empty(const struct hlist_head *head)
{
	return !head->first;
}

#define hlist_entry(node, type, member) container_of(node, type, member)

#ifdef LIST_TYPEOF_USE
#define hlist_entry_safe(node, type, member) __extension__ ({ \
	 __typeof__(node) __node = (node); \
	 !__node ? NULL : hlist_entry(__node, type, member); })
#else
#define hlist_entry_safe(node, type, member) \
	(node) ? hlist_entry(node, type, member) : NULL
#endif

#define hlist_for_each(node, head) \
	for (node = (head)->first; \
	     node; \
	     node = node->next)

#define hlist_for_each_entry_t(entry, head, type, member) \
	for (entry = hlist_entry_safe((head)->first, type, member); \
	     entry; \
	     entry = hlist_entry_safe(entry->member.next, type, member))

#ifdef LIST_TYPEOF_USE
#define hlist_for_each_entry(entry, head, member) \
	hlist_for_each_entry_t(entry, head, __typeof__(*entry), member)
#endif

#define hlist_for_each_safe(node, safe, head) \
	for (node = (head)->first; \
	     node && ((safe = node->next) || 1); \
	     node = safe)

#define hlist_for_each_entry_safe_t(entry, safe, head, type, member) \
	for (entry = hlist_entry_safe((head)->first, type, member); \
	     entry && ((safe = entry->member.next) || 1); \
	     entry = hlist_entry_safe(safe, type, member))

#ifdef LIST_TYPEOF_USE
#define hlist_for_each_entry_safe(entry, safe, head, member) \
	hlist_for_each_entry_safe_t(entry, safe, head, __typeof__(*entry),\
				    member)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __LINUX_LIKE_LIST_H__ */
