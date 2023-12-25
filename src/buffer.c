#include "includes/buffer.h"
#include "includes/algebra.h"
#include "includes/file_reader.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void editor_input_normal_mode(editor_t* editor, SDL_Event* event);
void editor_input_insert_mode(editor_t* editor, SDL_Event* event);

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
	buf->lines = calloc(buf->count, sizeof(line_t*));
	buf->lines[0] = line_init(line_cap);
	buf->cursor = vec2s(0, 0);

	return buf;
}

void buffer_insert(buffer_t* buf, char* input) {
	size_t input_size = strlen(input);
	line_t* line = buf->lines[buf->cursor.y];

	if (line->size + input_size >= line->cap) {
		line_resize(line);
	}

	if (line->size == 0 || line->size == buf->cursor.x) {
		line_append(line, input);
		buffer_move_cursor_by(buf, vec2s(input_size, 0.0));
		return;
	}

	for(size_t i = line->size; i > buf->cursor.x; i--) {
		line->chars[(i-1) + input_size] = line->chars[i-1];
	}
	line->size += input_size;
	
	for (size_t i = 0; i < input_size; i++) {
		line->chars[buf->cursor.x+i] = input[i];
	}

	buffer_move_cursor_by(buf, vec2s(input_size, 0.0));
}

void buffer_remove_front(buffer_t* buf) {
	line_t* line = buf->lines[buf->cursor.y];
	if (buf->cursor.x > line->size || (buf->cursor.x <= 0 && buf->cursor.y <= 0)) {
		return;
	}

	if (line->size == buf->cursor.x && buf->cursor.x != 0) {
		line_pop(line);
		buffer_move_cursor_by(buf, vec2s(-1, 0));
		return;
	} else if (buf->cursor.x == 0) {
		size_t dest_size = buf->lines[buf->cursor.y-1]->size;
		buffer_join_front(buf);
		buffer_move_cursor_to(buf, vec2s(dest_size, buf->cursor.y-1));
		return;
	}

	for (size_t i = buf->cursor.x; i < line->size; i++) {
		line->chars[i-1] = line->chars[i];
	}
	line->chars[line->size--] = 0;
	buffer_move_cursor_by(buf, vec2s(-1, 0));
}

void buffer_remove_back(buffer_t* buf) {
	line_t* line = buf->lines[buf->cursor.y];
	if (buf->cursor.x > line->size || (buf->cursor.x >= line->size && buf->count-1 <= buf->cursor.y)) {
		return;
	}

	if ((line->size-1) == buf->cursor.x) {
		line_pop(line);
		return;
	} else if (buf->cursor.x == line->size && buf->count-1 > buf->cursor.y) {
		buffer_join_back(buf);
		return;
	}

	for (size_t i = buf->cursor.x+1; i < line->size; i++) {
		line->chars[i-1] = line->chars[i];
	}
	line->chars[line->size--] = 0;
}

void buffer_move_cursor_by(buffer_t *buf, Vec2s movement) {
	buf->cursor = vec2s_add(buf->cursor, movement);
}

void buffer_move_cursor_to(buffer_t* buf, Vec2s pos) {
	buf->cursor = pos;
}

void buffer_new_line(buffer_t* buf) {
	line_t* cursor_line = buf->lines[buf->cursor.y];

	if (buf->cursor.y == buf->count-1 && 
		buf->cursor.x == cursor_line->size) {
		// When the cursor is at the end of the last line
		buf->count += 1;
		buf->lines = realloc(buf->lines, buf->count * sizeof(line_t*));
		buf->lines[buf->count-1] = line_init(80);
		buffer_move_cursor_to(buf, vec2s(0.0, buf->cursor.y+1));
	} else if (buf->cursor.x == cursor_line->size) {
		// When the cursor is at the end of a line
		buf->count += 1;
		buf->lines = realloc(buf->lines, buf->count * sizeof(line_t*));

		for (size_t i = buf->count-2; i > buf->cursor.y; i--) {
			buf->lines[i+1] = buf->lines[i];
		}

		buf->lines[buf->cursor.y+1] = line_init(80);
		buffer_move_cursor_to(buf, vec2s(0.0, buf->cursor.y+1));
	} else {
		// When the cursor is somewhere in a line
		line_t* current = buf->lines[buf->cursor.y];
		buf->count += 1;
		buf->lines = realloc(buf->lines, buf->count * sizeof(line_t*));

		for (size_t i = buf->count-2; i > buf->cursor.y; i--) {
			buf->lines[i+1] = buf->lines[i];
		}

		buf->lines[buf->cursor.y+1] = line_init(80);

		size_t chars_after_cursor = current->size - buf->cursor.x;
		memmove(buf->lines[buf->cursor.y+1]->chars, &current->chars[buf->cursor.x], chars_after_cursor);
		
		buf->lines[buf->cursor.y+1]->size = chars_after_cursor;
		current->size = buf->cursor.x;
		buffer_move_cursor_to(buf, vec2s(0.0, buf->cursor.y+1));
	}
}

void buffer_join_front(buffer_t* buf) {
	line_t* dst = buf->lines[buf->cursor.y-1];
	line_t* src = buf->lines[buf->cursor.y];

	if (dst->size + src->size > dst->cap) {
		line_resize(dst);
	}

	if (src->size > 0) {
		memmove(&dst->chars[dst->size], src->chars, src->size);
		dst->size += src->size;
	}

	for (size_t i = buf->cursor.y; i < buf->count-1; i++) {
		memmove(buf->lines[i], buf->lines[i+1], sizeof(line_t));
	}


	buf->count -= 1;
}

void buffer_join_back(buffer_t* buf) {
	line_t* dst = buf->lines[buf->cursor.y];
	line_t* src = buf->lines[buf->cursor.y+1];

	if (dst->size + src->size > dst->cap) {
		line_resize(dst);
	}

	if (src->size > 0) {
		memmove(&dst->chars[dst->size], src->chars, src->size);
		dst->size += src->size;
	}

	for (size_t i = buf->cursor.y+1; i < buf->count-1; i++) {
		memmove(buf->lines[i], buf->lines[i+1], sizeof(line_t));
	}

	buf->count -= 1;
}

float clamp_cursor_x(line_t* line, Vec2s cursor) {
	if (line->size < cursor.x) {
		return line->size;
	} else {
		return cursor.x;
	}
}

void editor_init(size_t line_cap, editor_t* editor, int w, int h) {
	editor->buf = buffer_init(line_cap);
	editor->scale = 3.0;
	editor->mode = NORMAL;

	editor_resize(editor, w, h);
}

void editor_resize(editor_t* editor, int w, int h) {
	editor->h = h;
	editor->w = w;

	editor->vp_h = (h - (editor->info_row_h * (editor->font_size.y * editor->scale)));
	editor->vp_w = (w - (editor->num_col_w * editor->font_size.x));

	editor->vp_origin.x = editor->num_col_w;
	editor->vp_origin.y = 0;

	editor->t = editor->vp_origin.y;
	editor->b = (editor->vp_h / (editor->font_size.y * editor->scale))-1;

	editor->l = editor->vp_origin.x;
	editor->r = (editor->vp_w / (editor->font_size.x * editor->scale))-1;

	editor_try_move_viewport(editor);
}

void editor_load_file(editor_t *editor, char *filepath) {
	editor->filepath = filepath;
	size_t line_count = 0;
	char** lines = read_lines(filepath, &line_count);

	if (line_count != 0) {
		buffer_insert(editor->buf, lines[0]);
		for (size_t i = 1; i < line_count-1; i++) {
			buffer_new_line(editor->buf);
			buffer_insert(editor->buf, lines[i]);
		}

		buffer_move_cursor_to(editor->buf, vec2s(0, 0));
	}

	size_t digit_count = 1;
	while (line_count != 0) {
		digit_count++;
		line_count /= 10;
	}

	editor->num_col_w = digit_count;
	editor_resize(editor, editor->w, editor->h);
}

void editor_write_file(editor_t* editor, char* filepath) {
	char** lines = calloc(editor->buf->count, sizeof(char*));
	for (size_t i = 0; i < editor->buf->count; i++) {
		line_t* line = editor->buf->lines[i];
		if (line->size != 0) {
			lines[i] = calloc(line->size+1, sizeof(char));

			memmove(lines[i], line->chars, line->size);
		} else {
			lines[i] = calloc(1, sizeof(char));
		}
	}

	write_lines(filepath, lines, editor->buf->count);
}

void editor_zoom(editor_t* editor, float mod) {
	float new_scale = editor->scale + mod;
	if (new_scale >= 1 && new_scale <= 15) {
		editor->scale = new_scale;
	}
}

void editor_move_cursor_by(editor_t* editor, Vec2s movement) {
	buffer_move_cursor_by(editor->buf, movement);

	editor_try_move_viewport(editor);
}

void editor_move_cursor_to(editor_t* editor, Vec2s pos) {
	buffer_move_cursor_to(editor->buf, pos);

	editor_try_move_viewport(editor);
}

void editor_try_move_viewport(editor_t* editor) {
	if (editor->buf->cursor.x > editor->r) {
		size_t diff = editor->buf->cursor.x - editor->r;
		editor_move_viewport_to(editor, editor->t, editor->b, editor->l+diff, editor->buf->cursor.x);
	} else if (editor->buf->cursor.x < editor->l) {
		size_t diff = editor->l - editor->buf->cursor.x;
		editor_move_viewport_to(editor, editor->t, editor->b, editor->buf->cursor.x, editor->r-diff);
	} else if (editor->buf->cursor.y < editor->t) {
		size_t diff = editor->t - editor->buf->cursor.y;
		editor_move_viewport_to(editor, editor->buf->cursor.y, editor->b-diff, editor->l, editor->r);
	} else if (editor->buf->cursor.y > editor->b) {
		size_t diff = editor->buf->cursor.y - editor->b;
		editor_move_viewport_to(editor, editor->t+diff, editor->buf->cursor.y, editor->l, editor->r);
	}
}

void editor_move_viewport_to(editor_t* editor, size_t t, size_t b, size_t l , size_t r) {
	editor->t = t;
	editor->b = b;
	editor->l = l;
	editor->r = r;
}

void editor_move_viewport_by(editor_t* editor, size_t t, size_t b, size_t l , size_t r) {
	editor_move_viewport_to(editor, editor->t+t, editor->b+b, editor->l+l, editor->r+r);
}

const char* editor_get_mode_string(editor_t* editor) {
	static const char* modes[2] = {
		"NORMAL",
		"INSERT",
	};

	return modes[editor->mode];
}

void editor_set_mode(editor_t* editor, editor_mode mode) {
	editor->mode = mode;
}

void editor_handle_events(editor_t* editor, SDL_Event* event, bool* quit) {
	if (event->type == SDL_QUIT) {
		*quit = true;
		return;
	}

	switch (editor->mode) {
		case NORMAL:
			editor_input_normal_mode(editor, event);
			break;

		case INSERT:
			editor_input_insert_mode(editor, event);
			break;

		case MODE_COUNT:
			assert(false && "MODE_COUNT is not an editor mode");
			break;
	}
}

void editor_input_normal_mode(editor_t* editor, SDL_Event* event) {
	switch (event->type) {
		case SDL_TEXTINPUT:
			break;

		case SDL_WINDOWEVENT:
			{
				switch (event->window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						editor_resize(editor, event->window.data1, event->window.data2);
				}
			}
			break;

		case SDL_KEYDOWN:
			{
				switch (event->key.keysym.sym) {
					case SDLK_h:
						if (editor->buf->cursor.x > 0) {
							editor_move_cursor_by(editor, vec2s(-1, 0));
						}
						break;
					case SDLK_l:
						if (editor->buf->cursor.x < editor->buf->lines[editor->buf->cursor.y]->size) {
							editor_move_cursor_by(editor, vec2s(1, 0));
						}
						break;
					case SDLK_j:
						if (editor->buf->cursor.y < editor->buf->count-1) {
							editor_move_cursor_to(editor, vec2s(clamp_cursor_x(editor->buf->lines[editor->buf->cursor.y+1], editor->buf->cursor), editor->buf->cursor.y+1.0));
						}
						break;
					case SDLK_k:
						if (editor->buf->cursor.y > 0) {
							editor_move_cursor_to(editor, vec2s(clamp_cursor_x(editor->buf->lines[editor->buf->cursor.y-1], editor->buf->cursor), editor->buf->cursor.y-1.0));
						}
						break;

					case SDLK_i: 
						editor_set_mode(editor, INSERT);
						break;

					case SDLK_w: {
							 if (event->key.keysym.mod & KMOD_CTRL) {
								 if (editor->filepath != NULL) {
									 editor_write_file(editor, editor->filepath);
									 printf("File saved!\n");
								 }
							 }
						 }
						 break;

					case SDLK_PLUS: {
							 if (event->key.keysym.mod & KMOD_CTRL) {
								editor_zoom(editor, 1.0);
								editor_resize(editor, editor->w, editor->h);
							}
						}
						break;

					case SDLK_MINUS: {
							 if (event->key.keysym.mod & KMOD_CTRL) {
								 editor_zoom(editor, -1.0);
								 editor_resize(editor, editor->w, editor->h);
							 }
						 }
						 break;

				}
			}
			break;

	}
}

void editor_input_insert_mode(editor_t* editor, SDL_Event* event) {
	switch (event->type) {
		case SDL_KEYDOWN:
			{
				if (event->key.keysym.mod == KMOD_LALT) {
					editor_input_normal_mode(editor, event);
					return;
				}

				switch (event->key.keysym.sym) {
					case SDLK_ESCAPE:
						editor_set_mode(editor, NORMAL);
						break;

					case SDLK_RETURN:
						buffer_new_line(editor->buf);
						editor_try_move_viewport(editor);
						break;

					case SDLK_BACKSPACE:
						buffer_remove_front(editor->buf);
						editor_try_move_viewport(editor);
						break;

					case SDLK_DELETE:
						buffer_remove_back(editor->buf);
						editor_try_move_viewport(editor);
						break;

					case SDLK_LEFT:
						if (editor->buf->cursor.x > 0) {
							editor_move_cursor_by(editor, vec2s(-1, 0));
						}
						break;

					case SDLK_RIGHT:
						if (editor->buf->cursor.x < editor->buf->lines[editor->buf->cursor.y]->size) {
							editor_move_cursor_by(editor, vec2s(1, 0));
						}
						break;

					case SDLK_DOWN:
						if (editor->buf->cursor.y < editor->buf->count-1) {
							editor_move_cursor_to(editor, vec2s(clamp_cursor_x(editor->buf->lines[editor->buf->cursor.y+1], editor->buf->cursor), editor->buf->cursor.y+1.0));
						}
						break;

					case SDLK_UP:
						if (editor->buf->cursor.y > 0) {
							editor_move_cursor_to(editor, vec2s(clamp_cursor_x(editor->buf->lines[editor->buf->cursor.y-1], editor->buf->cursor), editor->buf->cursor.y-1.0));
						}
						break;
				}
			}
			break;

		case SDL_TEXTINPUT:
			buffer_insert(editor->buf, event->text.text);
			editor_try_move_viewport(editor);
			break;
	}
}
