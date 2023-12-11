#include "includes/buffer.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"
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

void render_cursor(SDL_Renderer* renderer, Vec2f pos, Uint32 color, float scale) {
	const SDL_Rect cursor_rect = {
		.x = (int) floorf(pos.x),
		.y = (int) floorf(pos.y),
		.w = (int)floorf((float)FONT_CHAR_WIDTH * scale),
		.h = (int)floorf((float)FONT_CHAR_HEIGHT * scale),
	};

	SDL_SetRenderDrawColor(renderer, color , color >> (1*8), color >> (2*8), color >> (3*8));
	SDL_RenderFillRect(renderer, &cursor_rect);
}

void render_char(SDL_Renderer* renderer, SDL_Texture* font, char c, Vec2f pos, Uint32 color, float scale) {
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
		.x = (int)floorf(pos.x),
		.y = (int)floorf(pos.y),
		.w = (int)floorf((float)FONT_CHAR_WIDTH * scale),
		.h = (int)floorf((float)FONT_CHAR_HEIGHT * scale),
	};

	SDL_SetTextureColorMod(font, color , color >> (1*8), color >> (2*8));
	SDL_RenderCopy(renderer, font, &src, &dst);
}

void render_text(SDL_Renderer* renderer, SDL_Texture* font, const char* text, Vec2f pos, Uint32 color, float scale, Vec2f cursor) {
	size_t n = strlen(text);

	Vec2f pen = pos;
	for (size_t i = 0; i < n; i++) {
		render_char(renderer, font, text[i], pen, color, scale);
		pen.x += (float)FONT_CHAR_WIDTH * scale;
	}

	render_cursor(renderer, cursor, 0xFFFFFFFF, scale);
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Text editor",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			800, 600,
			SDL_WINDOW_RESIZABLE);
	assert(window != NULL);
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
	assert(renderer != NULL);

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

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	assert(texture != NULL);

	bool quit = false;

	buffer_t* buffer = buffer_init(1024);
	size_t cursor = 0;

	while (!quit) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_Event event = {0};
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_TEXTINPUT: {
						buffer_insert_at(buffer, event.text.text, cursor);
						cursor += strlen(event.text.text);
					}
					break;
			}
		}

		render_text(renderer, texture, buffer->text, vec2f(0.0, 0.0), 0xFF0000FF, 5.0f, vec2f(((float)cursor * FONT_CHAR_WIDTH * 5), 0.0));
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
