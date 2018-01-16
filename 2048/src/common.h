#pragma once
#include <SDL.h>

typedef struct {
	// Board has size*size elements
	uint32_t *board;
	uint8_t size;

	SDL_Renderer *renderer;
	int window_width, window_height;
} Game;

uint32_t get_value(const Game *game, int x, int y);