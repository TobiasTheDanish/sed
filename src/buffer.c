#include "includes/buffer.h"
#include "includes/algebra.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

buffer_t* buffer_init(size_t cap) {
	buffer_t* buf = malloc(sizeof(buffer_t));
	buf->size = 0;
	buf->cap = cap;
	buf->text = calloc(cap, sizeof(char));
	buf->cursor = vec2f(0.0, 0.0);

	return buf;
}

void buffer_append(buffer_t* buf, char* input) {
	size_t input_size = strlen(input);
	assert((buf->size + input_size) < buf->cap);

	for(size_t i = 0; i < input_size; i++) {
		buf->text[buf->size++] = input[i];
	}
	buffer_move_cursor(buf, vec2f(input_size, 0.0));
}

void buffer_pop(buffer_t* buf) {
	buf->size -= 1;
	buf->text[buf->size] = 0;
	buffer_move_cursor(buf, vec2f(-1.0, 0.0));
}

void buffer_insert(buffer_t* buf, char* input) {
	size_t input_size = strlen(input);
	assert((buf->size + input_size) < buf->cap);

	if (buf->size == 0 || buf->size == (size_t)roundf(buf->cursor.x)) {
		buffer_append(buf, input);
		return;
	}

	for(size_t i = buf->size-1; i >= (size_t)buf->cursor.x; i--) {
		buf->text[i + input_size] = buf->text[i];
	}

	size_t i = 0;
	while (i < input_size) {
		buf->text[(size_t)buf->cursor.x++] = input[i++];
	}
	buf->size += input_size;
}

void buffer_remove(buffer_t* buf) {
	if ((size_t)floorf(buf->cursor.x) > buf->size || (size_t)roundf(buf->cursor.x) <= 0) {
		return ;
	}

	if (buf->size == (size_t)roundf(buf->cursor.x)) {
		buffer_pop(buf);
		return;
	}

	for (size_t i = (size_t)roundf(buf->cursor.x); i < buf->size; i++) {
		buf->text[i-1] = buf->text[i];
	}
	buf->size -= 1;
	buf->text[buf->size] = 0;
	buffer_move_cursor(buf, vec2f(-1.0, 0.0));
}

void buffer_move_cursor(buffer_t *buf, Vec2f movement) {
	buf->cursor = vec2f_add(buf->cursor, movement);
}
