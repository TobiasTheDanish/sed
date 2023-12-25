#include "includes/buffer.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"
#include "includes/algebra.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define DEFAULT_WINDOW_WIDTH 1600
#define DEFAULT_WINDOW_HEIGHT 900

#define INFO_ROW_HEIGHT 2
#define NUM_COL_START 0
#define NUM_COL_WIDTH 5

#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_COLS 18
#define FONT_ROWS 7
#define FONT_CHAR_WIDTH (FONT_WIDTH / FONT_COLS)
#define FONT_CHAR_HEIGHT (FONT_HEIGHT / FONT_ROWS)

void render_cursor(SDL_Renderer* renderer, Vec2s font_size, Vec2s pos, Uint32 color, float scale) {
	const SDL_Rect cursor_rect = {
		.x = (int)((float)pos.x * font_size.x * scale),
		.y = (int)((float)pos.y * font_size.y * scale),
		.w = (int)floorf((float)font_size.x * scale),
		.h = (int)floorf((float)font_size.y * scale),
	};

	SDL_SetRenderDrawColor(renderer, color , color >> (1*8), color >> (2*8), color >> (3*8));
	SDL_RenderFillRect(renderer, &cursor_rect);
}

void render_char(SDL_Renderer* renderer, SDL_Texture* font, Vec2s font_size, char c, Vec2s pos, Uint32 color, float scale) {
	const size_t index = c - 32;
	const size_t col = index % FONT_COLS;
	const size_t row = index / FONT_COLS;

	const SDL_Rect src = {
		.x = col * font_size.x,
		.y = row * font_size.y,
		.w = font_size.x,
		.h = font_size.y,
	};

	const SDL_Rect dst = {
		.x = (int)((float)pos.x * font_size.x * scale),
		.y = (int)((float)pos.y * font_size.y * scale),
		.w = (int)floorf((float)font_size.x * scale),
		.h = (int)floorf((float)font_size.y * scale),
	};

	SDL_SetTextureColorMod(font, color , color >> (1*8), color >> (2*8));
	SDL_RenderCopy(renderer, font, &src, &dst);
}

void render_num_col(SDL_Renderer* renderer, SDL_Texture* font, editor_t* editor, Uint32 color) {
	size_t cursor_row_in_viewport = editor->buf->cursor.y;
	Vec2s pen = {
		.x = (editor->num_col_l + editor->num_col_w)-2,
		.y = 0,
	};

	for (size_t y = editor->t; y <= editor->b; y++) {
		if (y >= editor->buf->count) {
			break;
		}

		size_t current;
		if (y == cursor_row_in_viewport) {
			current = y+1;
			size_t digit_count = 0;
			while (current != 0) {
				digit_count++;
				current /= 10;
			}
			current = y+1;
			pen.x = digit_count-1;
		} else {
			current = abs((int)y - (int)cursor_row_in_viewport);
		}

		size_t index = current % 10;

		while (current != 0) {
			char c = (char)index + 48;
			render_char(renderer, font, editor->font_size, c, pen, color, editor->scale);

			current /= 10;
			index = current % 10;
			pen.x -= 1;
		}

		pen.x = (editor->num_col_l + editor->num_col_w)-2,
		pen.y += 1;
	}
}

void render_text(SDL_Renderer* renderer, SDL_Texture* font, editor_t* editor, Uint32 color) {
	Vec2s cursor_in_viewport = vec2s_sub(editor->buf->cursor, vec2s(editor->l, editor->t));
	cursor_in_viewport = vec2s_add(cursor_in_viewport, editor->vp_origin);

	render_cursor(renderer, editor->font_size, cursor_in_viewport, 0xFFFFFFFF, editor->scale);
	
	Vec2s pen = {
		.x = editor->vp_origin.x,
		.y = editor->vp_origin.y,
	};
	for (size_t y = editor->t; y < editor->buf->count; y++) {
		if (y > editor->b) {
			break;
		}

		line_t* line = editor->buf->lines[y];

		for (size_t x = editor->l; x < line->size; x++) {
			if (x > editor->r) {
				break;
			}

			if (vec2s_cmp(pen, cursor_in_viewport)) {
				render_char(renderer, font, editor->font_size, line->chars[x], pen, 0x00000000, editor->scale);
			} else {
				render_char(renderer, font, editor->font_size, line->chars[x], pen, color, editor->scale);
			}
			pen.x += 1;
		}
		pen.x = editor->vp_origin.x,
		pen.y += 1;
	}
}

void print_usage() {
	printf("Usage:\n");
	printf("     - sed <filepath> : opens editor at filepath\n");
}

int main(int argc, char** argv) {
	if (argc != 2) {
		print_usage();
		return 1;
	}
	char* filepath = argv[1];

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Text editor",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
			SDL_WINDOW_RESIZABLE);
	assert(window != NULL);
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
	assert(renderer != NULL);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int w,h,n;
    unsigned char* font = stbi_load("charmap-oldschool_white.png", &w, &h, &n, STBI_rgb_alpha);
	assert(font != NULL);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const Uint32 rmask = 0xFF000000;
	const Uint32 gmask = 0x00FF0000;
	const Uint32 bmask = 0x0000FF00;
	const Uint32 amask = 0x000000FF;
#else 
	const Uint32 rmask = 0x000000FF;
	const Uint32 gmask = 0x0000FF00;
	const Uint32 bmask = 0x00FF0000;
	const Uint32 amask = 0xFF000000;
#endif

	const int depth = 32;
	const int pitch = 4*w;

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(font, w, h, depth, pitch, rmask, gmask, bmask, amask);
	assert(surface != NULL);
	SDL_SetColorKey(surface, SDL_TRUE, 0x00000000);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	assert(texture != NULL);

	bool quit = false;

	editor_t editor = {0};
	editor.info_row_h = INFO_ROW_HEIGHT;
	editor.num_col_l = NUM_COL_START;
	editor.num_col_w = NUM_COL_WIDTH;
	editor.font_size.x = FONT_CHAR_WIDTH;
	editor.font_size.y = FONT_CHAR_HEIGHT;
	editor_init(80, &editor, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

	editor_load_file(&editor, filepath);

	while (!quit) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_Event event = {0};
		while(SDL_PollEvent(&event)) {
			editor_handle_events(&editor, &event, &quit);
		}

		render_num_col(renderer, texture, &editor, 0xFFFFFFFF);
		render_text(renderer, texture, &editor, 0xFFFFFFFF);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}



#define FILE_READER_IMPL
#include "includes/file_reader.h"
