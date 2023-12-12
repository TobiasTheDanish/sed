#include "includes/buffer.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

buffer_t* buffer_init(size_t cap) {
	buffer_t* buf = malloc(sizeof(buffer_t));
	buf->size = 0;
	buf->cap = cap;
	buf->text = calloc(cap, sizeof(char));

	return buf;
}

void buffer_append(buffer_t* buf, char* input) {
	size_t input_size = strlen(input);
	assert((buf->size + input_size) < buf->cap);

	for(size_t i = 0; i < input_size; i++) {
		buf->text[buf->size++] = input[i];
	}
}

int buffer_pop(buffer_t* buf) {
	buf->size -= 1;
	buf->text[buf->size] = 0;
	return 1;
}

void buffer_insert_at(buffer_t* buf, char* input, size_t pos) {
	size_t input_size = strlen(input);
	assert((buf->size + input_size) < buf->cap);

	if (buf->size == 0 || buf->size == pos) {
		buffer_append(buf, input);
		return;
	}

	for(size_t i = buf->size-1; i >= pos; i--) {
		buf->text[i + input_size] = buf->text[i];
	}

	size_t i = 0;
	while (i < input_size) {
		buf->text[pos++] = input[i++];
	}
	buf->size += input_size;
}

int buffer_remove_at(buffer_t* buf, size_t pos) {
	if (pos > buf->size || pos <= 0) {
		return 0;
	}

	if (buf->size == pos) {
		return buffer_pop(buf);
	}

	for (size_t i = pos; i < buf->size; i++) {
		buf->text[i-1] = buf->text[i];
	}
	buf->size -= 1;
	buf->text[buf->size] = 0;
	return 1;
}
