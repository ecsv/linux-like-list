// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>

int main(void)
{
	LIST_HEAD(testlist);

	assert(list_empty(&testlist));

	return 0;
}
