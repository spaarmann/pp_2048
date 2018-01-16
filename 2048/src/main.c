#include <stdio.h>
#include <SDL.h>

#include "common.h"
#include "renderer.h"

const int WIDTH = 840;
const int HEIGHT = 680;

int main(int argc, char **argv) {
	Game game;

	game.size = 4; // TODO: Make configurable?
	game.board = malloc(sizeof(uint32_t) * game.size * game.size);

	game.window_width = WIDTH;
	game.window_height = HEIGHT;
	game.renderer = create_renderer_and_window(WIDTH, HEIGHT);

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
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = 1;
				break;
			}
		}

		SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
		SDL_RenderClear(game.renderer);

		display_board(&game);

		SDL_RenderPresent(game.renderer);
	}

	free(game.board);

	return 0;
}