#include "renderer.h"

#include <stdio.h>
#include <SDL_ttf.h>

void sdl_error(char *msg) {
	printf("SDL Error (%s): %s\n", msg, SDL_GetError());
	SDL_Quit();
	// TODO: Resource cleanup
	exit(1);
}

SDL_Renderer *create_renderer_and_window(int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		sdl_error("SDL_Init");
	}

	if (TTF_Init() != 0) {
		sdl_error("TTF_Init");
	}

	SDL_Window *window = SDL_CreateWindow("2048",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, 0);

	if (window == NULL) {
		sdl_error("CreateWindow");
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		sdl_error("CreateRenderer");
	}

	return renderer;
}

void display_square(Game *game, const uint32_t val, const SDL_Rect *rect) {
	uint32_t color;
	char *txt;
	switch (val) {
	case 0:
		color = 0x000000FF;
		txt = "";
		break;
	case 2:
		color = 0xFF0000FF;
		txt = "2";
		break;
	case 4:
		color = 0x00FF00FF;
		txt = "4";
		break;
	case 8:
		color = 0x0000FFFF;
		txt = "8";
		break;
	case 16:
		color = 0xFFFFFFFF;
		txt = "16";
		break;
	}

	uint8_t r = color >> 24 & 0x000000FF;
	uint8_t g = (color >> 16) & 0x000000FF;
	uint8_t b = (color >> 8) & 0x000000FF;
	//uint8_t a = (color >> 0) & 0x000000FF;

	SDL_SetRenderDrawColor(game->renderer, r, g, b, 255);
	SDL_RenderFillRect(game->renderer, rect);

	// TODO: Don't redo all of this every frame
	TTF_Font *font = TTF_OpenFont("arial.ttf", 12);
	if (font == NULL) {
		sdl_error("TTF_OpenFont");
	}
	SDL_Color textColor = { 255 - r, 255 - g, 255 - b, 255};
	SDL_Surface *surface = TTF_RenderText_Solid(font, txt, textColor);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, surface);

	SDL_RenderCopy(game->renderer, tex, NULL, rect);
}

void display_board(Game *game) {
	int square_width = game->window_width / game->size;
	int square_height = game->window_height / game->size;

	for (int x = 0; x < game->size; x++) {
		for (int y = 0; y < game->size; y++) {
			SDL_Rect rect;
			rect.x = x * square_width;
			rect.y = y * square_height;
			rect.w = square_width;
			rect.h = square_height;

			display_square(game, get_value(game, x, y), &rect);
		}
	}
}

