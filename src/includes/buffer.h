#ifndef BUFFER_H
#define BUFFER_H

#include "algebra.h"
#include <stddef.h>

typedef struct {
	char* text;
	size_t size;
	size_t cap;
	Vec2f cursor;
} buffer_t;

buffer_t* buffer_init(size_t cap);
void buffer_insert(buffer_t* buf, char* input);
void buffer_remove(buffer_t* buf);
void buffer_move_cursor(buffer_t* buf, Vec2f movement);

#endif // !BUFFER_H
