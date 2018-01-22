#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct TileTexture {
	uint32_t val;
	SDL_Texture *texture;

	struct TileTexture *next;
} TileTexture;

enum GameState {
	StartScreen,
	InGame,
	EndScreen
};

enum GameMode {
	Normal,
	Endless
};

typedef struct {
	// Board has size*size elements
	uint32_t *board;
	uint8_t size;

	uint32_t score;

	enum GameState state;
	enum GameMode mode;

	SDL_Renderer *renderer;
	SDL_Window *window;
	TTF_Font *interface_font;

	TileTexture *tile_textures;

	SDL_Rect *start_normal_rect;
	SDL_Rect *start_endless_rect;
	SDL_Texture *startscreen_texture;

	SDL_Rect *play_again_rect;
	SDL_Texture *endscreen_texture;

	int window_width, window_height;
} Game;

uint32_t get_value(const Game *game, int x, int y);