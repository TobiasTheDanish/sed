#ifndef BUFFER_H
#define BUFFER_H

#include "algebra.h"
#include <stddef.h>

typedef struct {
	char* text;
	size_t size;
	size_t cap;
} buffer_t;

buffer_t* buffer_init(size_t cap);
void buffer_insert_at(buffer_t* buf, char* input, size_t pos);
int buffer_remove_at(buffer_t* buf, size_t pos);

#endif // !BUFFER_H
