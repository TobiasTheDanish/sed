#ifndef BUFFER_H
#define BUFFER_H

#include "algebra.h"
#include <stddef.h>
typedef struct {
	char* chars;
	size_t size;
	size_t cap;
} line_t;

typedef struct {
	line_t** lines;
	size_t count;
	Vec2f cursor;
} buffer_t;

line_t* line_init(size_t cap);
void line_resize(line_t* line);

buffer_t* buffer_init(size_t line_cap);
void buffer_insert(buffer_t* buf, char* input);
void buffer_remove(buffer_t* buf);
void buffer_move_cursor(buffer_t* buf, Vec2f movement);
void buffer_move_cursor_to(buffer_t* buf, Vec2f pos);
void buffer_new_line(buffer_t* buf);

float clamp_cursor_x(line_t* line, Vec2f cursor);
#endif // !BUFFER_H
