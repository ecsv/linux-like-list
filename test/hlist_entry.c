// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>
#include <stddef.h>

#include "common.h"

int main(void)
{
	struct hlistitem item;

	assert(&item == hlist_entry(&item.list, struct hlistitem, list));

	return 0;
}
