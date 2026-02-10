#include <stdio.h>

#include "ringbuffer.h"

int main()
{
	struct ringbuffer rb = {};

	/* Initialize the ringbuffer */

	rb_alloc(&rb, 8, sizeof(int));

	/* Fill the ringbuffer with a few elements */

	for (size_t i = 0; i < 8; i++)
		rb_push(&rb, &i);

	/* Print the ringbuffer */

	int *p = NULL;
	for (size_t i = 0; i < 2 * 8; i++) {
		p = rb_at(&rb, i);
		printf("%d\n", *p);
	}

	rb_free(&rb);

	return 0;
}
