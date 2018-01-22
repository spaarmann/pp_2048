#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#include "common.h"
#include "renderer.h"
#include "game.h"

const int WIDTH = 600;
const int HEIGHT = 700;

void init_board(Game *game) {
	game->score = 0;

	for (int i = 0; i < (game->size * game->size); i++) {
		game->board[i] = 0;
	}

	add_new_number(game);
	add_new_number(game);
}

int main(int argc, char **argv) {
	Game game;

	game.size = 4;
	game.board = malloc(sizeof(uint32_t) * game.size * game.size);
	game.state = StartScreen;

	game.window_width = WIDTH;
	game.window_height = HEIGHT;

	create_renderer_and_window(&game, WIDTH, HEIGHT);
	create_initial_tile_textures(&game);

	SDL_Event e;
	int quit = 0;

	while (!quit) {
		SDL_SetRenderDrawColor(game.renderer, 178, 125, 73, 255);
		SDL_RenderClear(game.renderer);

		switch (game.state) {
		case StartScreen:
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_MOUSEBUTTONUP: {
					SDL_Point p = { e.button.x, e.button.y };
					if (SDL_PointInRect(&p, game.start_normal_rect)) {
						init_board(&game);
						game.mode = Normal;
						game.state = InGame;
					}
					else if (SDL_PointInRect(&p, game.start_endless_rect)) {
						init_board(&game);
						game.mode = Endless;
						game.state = InGame;
					}
					break;
				}
				}
			}

			display_startscreen(&game);

			break;
		case InGame:
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN: {
					bool moved_anything = false;
					switch (e.key.keysym.sym) {
					case SDLK_UP:
						// Move Up
						moved_anything = move_up(&game);
						break;
					case SDLK_DOWN:
						// Move Down
						moved_anything = move_down(&game);
						break;
					case SDLK_LEFT:
						// Move Left
						moved_anything = move_left(&game);
						break;
					case SDLK_RIGHT:
						// Move Right
						moved_anything = move_right(&game);
						break;
					}

					if (moved_anything) {
						add_new_number(&game);
					}

					if (!has_valid_moves(&game) || has_won(&game)) {
						game.state = EndScreen;
					}
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
				case SDL_MOUSEBUTTONUP: {
					SDL_Point p = { e.button.x, e.button.y };
					if (SDL_PointInRect(&p, game.play_again_rect)) {
						init_board(&game);
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