// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>

int main(void)
{
	struct hlist_head testlist;

	INIT_HLIST_HEAD(&testlist);
	assert(hlist_empty(&testlist));

	return 0;
}
