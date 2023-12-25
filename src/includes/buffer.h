#ifndef BUFFER_H
#define BUFFER_H

#include "algebra.h"
#include <SDL2/SDL_events.h>
#include <stddef.h>

typedef enum {
	NORMAL = 0,
	INSERT = 1,
	MODE_COUNT = 2,
} editor_mode;

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
	int w, h;
	size_t t, b, l, r;
	size_t vp_w, vp_h;
	Vec2s vp_origin;
	Vec2s font_size;
	size_t num_col_l, num_col_w;
	size_t info_row_h;
	editor_mode mode;
	char* filepath;
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

void editor_init(size_t line_cap, editor_t* editor,int w, int h);
void editor_resize(editor_t* editor, int w, int h);
void editor_load_file(editor_t* editor, char* filepath);
void editor_write_file(editor_t* editor, char* filepath);
void editor_zoom(editor_t* editor, float mod);
void editor_move_cursor_by(editor_t* editor, Vec2s movement);
void editor_move_cursor_to(editor_t* editor, Vec2s pos);
void editor_try_move_viewport(editor_t* editor);
void editor_move_viewport_by(editor_t* editor, size_t t, size_t b, size_t l , size_t r);
void editor_move_viewport_to(editor_t* editor, size_t t, size_t b, size_t l , size_t r);
const char* editor_get_mode_string(editor_t* editor);
void editor_set_mode(editor_t* editor, editor_mode mode);
void editor_handle_events(editor_t* editor, SDL_Event* event, bool* quit);

#endif // !BUFFER_H
