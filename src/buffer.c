#include "includes/buffer.h"
#include "includes/algebra.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

line_t* line_init(size_t cap) {
	line_t* line = malloc(sizeof(line_t));
	line->cap = cap;
	line->size = 0;
	line->chars = calloc(cap, sizeof(char));

	return line;
}

void line_resize(line_t *line) {
	line->cap *= 2;
	line->chars = realloc(line->chars, line->cap * sizeof(char));
}

void line_append(line_t* line, char* input) {
	size_t input_size = strlen(input);

	for(size_t i = 0; i < input_size; i++) {
		line->chars[line->size++] = input[i];
	}
}

void line_pop(line_t* line) {
	line->chars[line->size--] = 0;
}

buffer_t* buffer_init(size_t line_cap) {
	buffer_t* buf = malloc(sizeof(buffer_t));
	buf->count = 1;
	buf->lines = malloc(sizeof(line_t*));
	buf->lines[0] = line_init(line_cap);
	buf->cursor = vec2f(0.0, 0.0);

	return buf;
}

void buffer_insert(buffer_t* buf, char* input) {
	size_t input_size = strlen(input);
	line_t* line = buf->lines[(size_t)buf->cursor.y];

	if (line->size + input_size >= line->cap) {
		line_resize(line);
	}

	if (line->size == 0 || line->size == (size_t)roundf(buf->cursor.x)) {
		line_append(line, input);
		buffer_move_cursor(buf, vec2f(input_size, 0.0));
		return;
	}

	for(size_t i = line->size; i > (size_t)buf->cursor.x; i--) {
		line->chars[(i-1) + input_size] = line->chars[i-1];
	}
	line->size += input_size;
	
	for (size_t i = 0; i < input_size; i++) {
		line->chars[(size_t)buf->cursor.x+i] = input[i];
	}

	buffer_move_cursor(buf, vec2f(input_size, 0.0));
}

void buffer_remove(buffer_t* buf) {
	line_t* line = buf->lines[(size_t)buf->cursor.y];
	if ((size_t)floorf(buf->cursor.x) > line->size) {
		return;
	}

	if (line->size == (size_t)roundf(buf->cursor.x)) {
		line_pop(line);
		buffer_move_cursor(buf, vec2f(-1.0, 0.0));
		return;
	} else if ((size_t)roundf(buf->cursor.x) <= 0) {
		buffer_join_lines(buf, buf->lines[(size_t)buf->cursor.y-1], buf->lines[(size_t)buf->cursor.y]);
		return;
	}

	for (size_t i = (size_t)roundf(buf->cursor.x); i < line->size; i++) {
		line->chars[i-1] = line->chars[i];
	}
	line->chars[line->size--] = 0;
	buffer_move_cursor(buf, vec2f(-1.0, 0.0));
}

void buffer_move_cursor(buffer_t *buf, Vec2f movement) {
	buf->cursor = vec2f_add(buf->cursor, movement);
}

void buffer_move_cursor_to(buffer_t* buf, Vec2f pos) {
	buf->cursor = pos;
}

void buffer_new_line(buffer_t* buf) {
	line_t* cursor_line = buf->lines[(size_t)buf->cursor.y];

	if (buf->cursor.y == buf->count-1 && 
		buf->cursor.x == cursor_line->size) {
		// When the cursor is at the end of the last line
		buf->count += 1;
		buf->lines = realloc(buf->lines, buf->count * sizeof(line_t*));
		buf->lines[buf->count-1] = line_init(80);
		buffer_move_cursor_to(buf, vec2f(0.0, buf->cursor.y+1));
	} else if (buf->cursor.x == cursor_line->size) {
		// When the cursor is at the end of a line
		buf->count += 1;
		buf->lines = realloc(buf->lines, buf->count * sizeof(line_t*));

		for (size_t i = buf->count-2; i > (size_t)buf->cursor.y; i--) {
			buf->lines[i+1] = buf->lines[i];
		}

		buf->lines[(size_t)buf->cursor.y+1] = line_init(80);
		buffer_move_cursor_to(buf, vec2f(0.0, buf->cursor.y+1));
	} else {
		// When the cursor is somewhere in a line
		line_t* current = buf->lines[(size_t)buf->cursor.y];
		buf->count += 1;
		buf->lines = realloc(buf->lines, buf->count * sizeof(line_t*));

		for (size_t i = buf->count-2; i > (size_t)buf->cursor.y; i--) {
			buf->lines[i+1] = buf->lines[i];
		}

		buf->lines[(size_t)buf->cursor.y+1] = line_init(80);

		size_t chars_after_cursor = current->size - (size_t)buf->cursor.x;
		memmove(buf->lines[(size_t)buf->cursor.y+1]->chars, &current->chars[(size_t)buf->cursor.x], chars_after_cursor);
		
		buf->lines[(size_t)buf->cursor.y+1]->size = chars_after_cursor;
		current->size = (size_t)buf->cursor.x;
		buffer_move_cursor_to(buf, vec2f(0.0, buf->cursor.y+1));
	}
}

void buffer_join_lines(buffer_t* buf, line_t* a, line_t* b) {
	if (a->size + b->size > a->cap) {
		line_resize(a);
	}

	memmove(&a->chars[a->size], b->chars, b->size);

	for (size_t i = (size_t)buf->cursor.y; i < buf->count-1; i++) {
		buf->lines[i] = buf->lines[i+1];
	}
	buffer_move_cursor_to(buf, vec2f(a->size, buf->cursor.y-1));
	a->size += b->size;

	buf->count -= 1;
	free(buf->lines[buf->count]);
	buf->lines[buf->count] = NULL;

}

float clamp_cursor_x(line_t* line, Vec2f cursor) {
	if (line->size < cursor.x) {
		return line->size;
	} else {
		return cursor.x;
	}
}
