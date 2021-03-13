// SPDX-License-Identifier: MIT
/* Minimal Linux-like double-linked list helper functions test
 *
 * SPDX-FileCopyrightText: Sven Eckelmann <sven@narfation.org>
 */

#include "../list.h"
#include <assert.h>
#include <stddef.h>

struct teststruct {
	int a;
	int b;
};

int main(void)
{
	struct teststruct item;

	assert(&item == container_of(&item.b, struct teststruct, b));

	return 0;
}
