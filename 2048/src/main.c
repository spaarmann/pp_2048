#include <stdio.h>
#include <SDL.h>

#include "common.h"
#include "renderer.h"

const int WIDTH = 600;
const int HEIGHT = 700;

int main(int argc, char **argv) {
	Game game;

	game.size = 4; // TODO: Make configurable?
	game.board = malloc(sizeof(uint32_t) * game.size * game.size);
	game.score = 0;
	game.state = InGame;

	game.window_width = WIDTH;
	game.window_height = HEIGHT;
	create_renderer_and_window(&game, WIDTH, HEIGHT);
	create_initial_tile_textures(&game);

	for (int x = 0; x < game.size; x++) {
		for (int y = 0; y < game.size; y++) {
			uint32_t val;

			if (x == y) val = 0;
			else if (x < 2 && y < 2) val = 2;
			else if (x < 2) val = 4;
			else if (y < 2) val = 8;
			else val = 16;

			game.board[y + x * game.size] = val;
		}
	}

	SDL_Event e;
	int quit = 0;

	while (!quit) {
		SDL_SetRenderDrawColor(game.renderer, 178, 125, 73, 255);
		SDL_RenderClear(game.renderer);

		switch (game.state) {
		case InGame:
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
					case SDLK_s:
						game.score++;
						break;
					case SDLK_e:
						game.state = EndScreen;
					case SDLK_UP:
						// Move Up
						break;
					case SDLK_DOWN:
						// Move Down
						break;
					case SDLK_LEFT:
						// Move Left
						break;
					case SDLK_RIGHT:
						// Move Right
						break;
					}
				}
			}

			display_interface(&game);
			display_board(&game);
			break;
		case EndScreen:
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
					case SDLK_e:
						game.state = InGame;
						break;
					}
				case SDL_MOUSEBUTTONUP: {
					SDL_Point p = { e.button.x, e.button.y };
					if (SDL_PointInRect(&p, game.play_again_rect)) {
						game.score = 0;
						// TODO: Generate new board
						game.state = InGame;
					}
					break;
				}
				}
			}

			display_endscreen(&game);

			break;
		}

		SDL_RenderPresent(game.renderer);
	}

	free_rendering_stuff(&game);
	free(game.board);

	return 0;
}