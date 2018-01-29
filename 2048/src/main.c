#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>

#include "common.h"
#include "renderer.h"
#include "game.h"

const int WIDTH = 600;
const int HEIGHT = 700;

// Resets the board to a starting state.
void init_board(Game *game) {
	game->score = 0;

	for (int i = 0; i < (game->size * game->size); i++) {
		game->board[i] = 0;
	}

	add_new_number(game);
	add_new_number(game);
}

int main(int argc, char **argv) {
	// Seed random number generator.
	srand(time(NULL));

	// Create our main game struct.
	Game game;

	game.size = 4;
	game.board = malloc(sizeof(uint32_t) * game.size * game.size);
	game.state = StartScreen;

	game.window_width = WIDTH;
	game.window_height = HEIGHT;

	// Create window and initialise some rendering stuff.
	create_renderer_and_window(&game, WIDTH, HEIGHT);
	create_initial_tile_textures(&game);

	SDL_Event e;
	int quit = 0;

	// Main Game Loop
	while (!quit) {
		// Clear screen
		SDL_SetRenderDrawColor(game.renderer, 178, 125, 73, 255);
		SDL_RenderClear(game.renderer);

		// Execute different code based on the current state of the game.
		switch (game.state) {
		case StartScreen:
			// Handle SDL events.
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_MOUSEBUTTONUP: {
					// Pressed the mouse, check if it was on the
					// "Normal" or "Endless" buttons.
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

			// Render the buttons.
			display_startscreen(&game);

			break;
		case InGame:
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN: {
					// Keyboard input
					bool moved_anything = false;
					switch (e.key.keysym.sym) {
					case SDLK_UP:
						// Move Up
						moved_anything = do_move_up(&game);
						break;
					case SDLK_DOWN:
						// Move Down
						moved_anything = do_move_down(&game);
						break;
					case SDLK_LEFT:
						// Move Left
						moved_anything = do_move_left(&game);
						break;
					case SDLK_RIGHT:
						// Move Right
						moved_anything = do_move_right(&game);
						break;
					}

					if (moved_anything) {
						// Add a new number if a move was executed.
						add_new_number(&game);
					}

					if (!has_valid_moves(&game) || has_won(&game)) {
						// Change to end screen if we lost or won.
						game.state = EndScreen;
					}
				}
				}
			}

			// Display the score text.
			display_interface(&game);
			// Display the board.
			display_board(&game);
			break;
		case EndScreen:
			// Like the start screen above.
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

			// Display the play again button.
			display_endscreen(&game);

			break;
		}

		// Actually show our rendered image on the screen.
		SDL_RenderPresent(game.renderer);
	}

	// When we exit, don't forget to clean up our resources.
	free_rendering_stuff(&game);
	free(game.board);

	return 0;
}