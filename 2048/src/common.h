#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct _tile_texture {
	uint32_t val;
	SDL_Texture *texture;

	struct _tile_texture *next;
} TileTexture;

enum GameState {
	InGame,
	EndScreen
};

typedef struct {
	// Board has size*size elements
	uint32_t *board;
	uint8_t size;

	uint32_t score;

	enum GameState state;

	SDL_Renderer *renderer;
	SDL_Window *window;
	TTF_Font *interface_font;

	TileTexture *tile_textures;

	SDL_Rect *play_again_rect;
	SDL_Texture *play_again_texture;

	int window_width, window_height;
} Game;

uint32_t get_value(const Game *game, int x, int y);