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
	struct hlistitem item;

	INIT_HLIST_HEAD(&testlist);
	assert(hlist_empty(&testlist));

	hlist_add_head(&item.list, &testlist);
	assert(!hlist_empty(&testlist));

	hlist_del(&item.list);
	assert(hlist_empty(&testlist));

	return 0;
}
