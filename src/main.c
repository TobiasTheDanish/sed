#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "includes/stb_image.h"

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

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
    unsigned char* font = stbi_load("KenPixel.png", &w, &h, &n, 0);
	assert(font != NULL);

	bool quit = false;

	while (!quit) {
		SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
		SDL_RenderClear(renderer);

		SDL_Event event = {0};
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
			}
		}

		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(font, w, h, 32, 0, 0, 0, 0, 0);
		assert(surface != NULL);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		assert(texture != NULL);
		SDL_Rect tex_rect;
		tex_rect.x = 0;
		tex_rect.y = 0;
		tex_rect.h = h;
		tex_rect.w = w;

		SDL_SetTextureColorMod(texture, 0xFF, 0xFF, 0xFF);
		SDL_RenderCopy(renderer, texture, &tex_rect, &tex_rect);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
