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
#include <stddef.h>

#include "common.h"

int main(void)
{
	struct list_head testlist;
	struct listitem item[4];
	size_t i;

	for (i = 0; i < sizeof(item) / sizeof(*item); i++)
		item[i].i = (int)i;

	INIT_LIST_HEAD(&testlist);
	assert(list_empty(&testlist));

	list_add_tail(&item[0].list, &testlist);
	assert(!list_empty(&testlist));
	assert(list_last_entry(&testlist, struct listitem, list)->i == 0);

	list_add(&item[1].list, &testlist);
	assert(!list_empty(&testlist));
	assert(list_last_entry(&testlist, struct listitem, list)->i == 0);

	list_add_tail(&item[2].list, &testlist);
	assert(!list_empty(&testlist));
	assert(list_last_entry(&testlist, struct listitem, list)->i == 2);

	list_add_tail(&item[3].list, &testlist);
	assert(!list_empty(&testlist));
	assert(list_last_entry(&testlist, struct listitem, list)->i == 3);

	return 0;
}
