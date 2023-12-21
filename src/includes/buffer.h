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
	Vec2s cursor;
} buffer_t;

typedef struct {
	buffer_t* buf;
	float scale;
	size_t t, b, l, r;
} editor_t;

line_t* line_init(size_t cap);
void line_resize(line_t* line);

buffer_t* buffer_init(size_t line_cap);
void buffer_insert(buffer_t* buf, char* input);
void buffer_remove_front(buffer_t* buf);
void buffer_remove_back(buffer_t* buf);
void buffer_move_cursor_by(buffer_t* buf, Vec2s movement);
void buffer_move_cursor_to(buffer_t* buf, Vec2s pos);
void buffer_new_line(buffer_t* buf);
void buffer_join_front(buffer_t* buf);
void buffer_join_back(buffer_t* buf);

float clamp_cursor_x(line_t* line, Vec2s cursor);

void editor_init(size_t line_cap, editor_t* editor, size_t b, size_t r);
void editor_load_file(editor_t* editor, char* filepath);
void editor_write_file(editor_t* editor, char* filepath);
void editor_zoom(editor_t* editor, float mod);
void editor_move_cursor_by(editor_t* editor, Vec2s movement);
void editor_move_cursor_to(editor_t* editor, Vec2s pos);
void editor_move_viewport_by(editor_t* editor, size_t t, size_t b, size_t l , size_t r);
void editor_move_viewport_to(editor_t* editor, size_t t, size_t b, size_t l , size_t r);

#endif // !BUFFER_H
