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

#include "common.h"

int main(void)
{
	struct list_head testlist;
	struct listitem item1;
	struct listitem item2;
	struct listitem item3;
	struct listitem item4;
	struct listitem *item;
	size_t i;

	item1.i = 1;
	item2.i = 2;
	item3.i = 3;
	item4.i = 4;

	INIT_LIST_HEAD(&testlist);
	assert(list_empty(&testlist));

	list_add(&item1.list, &testlist);
	list_add_behind(&item3.list, &item1.list);
	list_add_behind(&item2.list, &item1.list);
	list_add_behind(&item4.list, &item3.list);

	i = 1;
	list_for_each_entry_t(item, &testlist, struct listitem, list) {
		assert(item->i == i);
		i++;
	}
	assert(i == 5);

	return 0;
}
