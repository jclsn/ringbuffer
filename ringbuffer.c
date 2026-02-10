#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringbuffer.h"

/*
 * Allocates the ringbuffer
 */

int rb_alloc(struct ringbuffer *rb, const size_t size, const size_t elem_size)
{
	if (!rb || size == 0 || elem_size == 0) {
		errno = EINVAL;
		perror(__func__);
		return -1;
	}

	struct ringbuffer rb_temp = { .size = size, .elem_size = elem_size };
	memcpy(rb, &rb_temp, sizeof(struct ringbuffer));
	if (!rb) {
		perror(__func__);
		return -1;
	}
	rb->current = rb->size - 1;

	rb->data = calloc(size, elem_size);
	if (!rb->data) {
		perror(__func__);
		return -1;
	}

	return 0;
}

/*
 * Initializes the ringbuffer
 */

void rb_free(struct ringbuffer *rb)
{
	if (!rb || !rb->data)
		return;

	rb_reset(rb);
	free(rb->data);
	rb->data = NULL;
}

/*
 * Gets the sample at index i in the ringbuffer
 */

void *rb_at(const struct ringbuffer *rb, ptrdiff_t i)
{
	if (!rb || !rb->data || rb->size == 0) {
		errno = EINVAL;
		perror(__func__);
		return NULL;
	}

	ptrdiff_t index = (rb->current + i) % (ptrdiff_t)rb->size;
	while (index < 0)
		index += rb->size;

	/* Offset from the start in bytes: index * sizeof(element) */

	return (char *)rb->data + index * rb->elem_size;
}

/*
 * Decrements the ringbuffer pointer
 */

static inline void rb_dec(struct ringbuffer *rb)
{
	if (!rb || rb->size == 0) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	rb->current--;
	while (rb->current < 0)
		rb->current += rb->size;
}

/*
 * Pushes a new sample to the ringbuffer
 */

void rb_push(struct ringbuffer *rb, const void *elem)
{
	if (!rb || !rb->data) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	rb_dec(rb);

	/* Compute the destination pointer as an offset in bytes from the start of the buffer */

	memcpy((char *)rb->data + rb->current * rb->elem_size, elem, rb->elem_size);
}

/*
 * Fills the ringbuffer data with zeros and resets the pointer
 */

void rb_reset(struct ringbuffer *rb)
{
	if (!rb || !rb->data) {
		errno = EINVAL;
		perror(__func__);
		return;
	}

	rb->current = rb->size - 1;
	memset(rb->data, 0, rb->size * rb->elem_size);
}
