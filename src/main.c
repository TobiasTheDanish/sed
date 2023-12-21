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
#define FILE_READER_IMPL
#include "includes/file_reader.h"
#include "includes/algebra.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#define FONT_WIDTH 128
#define FONT_HEIGHT 64
#define FONT_COLS 18
#define FONT_ROWS 7
#define FONT_CHAR_WIDTH FONT_WIDTH / FONT_COLS
#define FONT_CHAR_HEIGHT FONT_HEIGHT / FONT_ROWS

void render_cursor(SDL_Renderer* renderer, Vec2s pos, Uint32 color, float scale) {
	const SDL_Rect cursor_rect = {
		.x = (int) (floorf(pos.x) * FONT_CHAR_WIDTH * scale),
		.y = (int) (floorf(pos.y) * FONT_CHAR_HEIGHT * scale),
		.w = (int)floorf((float)FONT_CHAR_WIDTH * scale),
		.h = (int)floorf((float)FONT_CHAR_HEIGHT * scale),
	};

	SDL_SetRenderDrawColor(renderer, color , color >> (1*8), color >> (2*8), color >> (3*8));
	SDL_RenderFillRect(renderer, &cursor_rect);
}

void render_char(SDL_Renderer* renderer, SDL_Texture* font, char c, Vec2s pos, Uint32 color, float scale) {
	const size_t index = c - 32;
	const size_t col = index % FONT_COLS;
	const size_t row = index / FONT_COLS;

	const SDL_Rect src = {
		.x = col * FONT_CHAR_WIDTH,
		.y = row * FONT_CHAR_HEIGHT,
		.w = FONT_CHAR_WIDTH,
		.h = FONT_CHAR_HEIGHT,
	};

	const SDL_Rect dst = {
		.x = (int)(floorf(pos.x) * FONT_CHAR_WIDTH * scale),
		.y = (int)(floorf(pos.y) * FONT_CHAR_HEIGHT * scale),
		.w = (int)floorf((float)FONT_CHAR_WIDTH * scale),
		.h = (int)floorf((float)FONT_CHAR_HEIGHT * scale),
	};

	SDL_SetTextureColorMod(font, color , color >> (1*8), color >> (2*8));
	SDL_RenderCopy(renderer, font, &src, &dst);
}

void render_text(SDL_Renderer* renderer, SDL_Texture* font, buffer_t* buf, Uint32 color, float scale) {
	render_cursor(renderer, buf->cursor, 0xFFFFFFFF, scale);
	
	Vec2s pen = vec2s(0.0, 0.0);
	for (size_t i = 0; i < buf->count; i++) {
		line_t* line = buf->lines[pen.y];

		for (size_t i = 0; i < line->size; i++) {
			if (vec2s_cmp(pen, buf->cursor)) {
				render_char(renderer, font, line->chars[i], pen, 0x00000000, scale);
			} else {
				render_char(renderer, font, line->chars[i], pen, color, scale);
			}
			pen.x += 1;
		}
		pen.x = 0;
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
			800, 600,
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
	editor_init(80, &editor);

	editor_load_file(&editor, filepath);

	while (!quit) {
		bool didZoom = false;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_Event event = {0};
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_KEYDOWN:
					{
						switch (event.key.keysym.sym) {
							case SDLK_RETURN:
								buffer_new_line(editor.buf);
								break;
							case SDLK_BACKSPACE:
								buffer_remove_front(editor.buf);
								break;
							case SDLK_DELETE:
								buffer_remove_back(editor.buf);
								break;
							case SDLK_LEFT:
								if (editor.buf->cursor.x > 0) {
									editor_move_cursor_by(&editor, vec2s(-1, 0));
								}
								break;
							case SDLK_RIGHT:
								if (editor.buf->cursor.x < editor.buf->lines[editor.buf->cursor.y]->size) {
									editor_move_cursor_by(&editor, vec2s(1, 0));
								}
								break;
							case SDLK_DOWN:
								if (editor.buf->cursor.y < editor.buf->count-1) {
									editor_move_cursor_to(&editor, vec2s(clamp_cursor_x(editor.buf->lines[editor.buf->cursor.y+1], editor.buf->cursor), editor.buf->cursor.y+1.0));
								}
								break;
							case SDLK_UP:
								if (editor.buf->cursor.y > 0) {
									editor_move_cursor_to(&editor, vec2s(clamp_cursor_x(editor.buf->lines[editor.buf->cursor.y-1], editor.buf->cursor), editor.buf->cursor.y-1.0));
								}
								break;
							case SDLK_w: {
									if (event.key.keysym.mod == KMOD_LCTRL) {
										if (filepath != NULL) {
											editor_write_file(&editor, filepath);
										}
									}
								}
								break;

							case SDLK_PLUS: {
									if (event.key.keysym.mod == KMOD_LCTRL) {
										editor_zoom(&editor, 1.0);
										didZoom = true;
									}
								}
								break;

							case SDLK_MINUS: {
									if (event.key.keysym.mod == KMOD_LCTRL) {
										editor_zoom(&editor, -1.0);
										didZoom = true;
									}
								}
								break;

						}
					}
					break;

				case SDL_TEXTINPUT:
					if (!((strcmp(event.text.text, "+") == 0 || strcmp(event.text.text, "-") == 0) && didZoom)) {
						buffer_insert(editor.buf, event.text.text);
					}
					break;
			}
		}

		render_text(renderer, texture, editor.buf, 0xFFFFFFFF, editor.scale);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
