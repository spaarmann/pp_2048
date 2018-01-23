#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#include "common.h"

// The game logic works by only performing moves upward.
// For other directions, the board is first rotated, then the move
// is executed and then the board is rotated back.

// Many of these functions work on a single column of the board,
// since for upward movement only the tiles in the same column are relevant.

// Finds a new position for the tile at 'x' after a move.
// 'stop' is used to prevent a tile being merged twice in one move.
uint32_t find_target(uint32_t *array, uint32_t x, uint32_t stop) {
	int32_t t;
	if (x == 0) {
		// Don't move empty tiles
		return x;
	}

	// Move upward starting at our tile.
	for (t = x - 1; t >= 0; t--) {
		if (array[t] != 0) {
			// The first field that is not empty.

			if (array[t] != array[x]) {
				// merge is not possible, take next position
				return t + 1;
			}

			// Merge is possible
			return t;
		}
		else {	
			if ((uint32_t) t == stop) {
				// we should not slide further, return this one
				return t;
			}
		}
	}
	// we did not find a new position
	return x;
}

// Executes a move for one column of the board.
// Returns whether any field was moved.
bool slide_array(Game *game, uint32_t *array) {
	bool success = false;
	uint32_t x, t, stop = 0;

	for (x = 0; x < game->size; x++) {
		// Ignore empty fields
		if (array[x] != 0) {
			// Find a new position for the field
			t = find_target(array, x, stop);
			if (t != x) { // if target is not original position, then move or merge
				if (array[t] == 0) { // if target is zero, this is a move
					array[t] = array[x];
				}
				else if (array[t] == array[x]) {
					array[t] *= 2; // merge (increase power of two)
					game->score += array[t];
					stop = t + 1; // set stop to avoid double merge
				}
				array[x] = 0;
				success = true;
			}
		}
	}
	return success;
}

// Rotates the board clockwise by 90°
void move_board(Game *game) {
	uint32_t i, j, n = game->size;
	uint32_t tmp;

	uint32_t *board = game->board;

	for (i = 0; i < n / 2; i++) {
		for (j = i; j < n - i - 1; j++) {
			tmp = board[j + i * game->size];
			board[j + i * game->size] = board[j * game->size + n - i - 1];
			board[j * game->size + n - i - 1] = board[(n - i - 1) * game->size + n - j - 1];
			board[(n - i - 1) * game->size + n - j - 1] = board[(n - j - 1) * game->size + i];
			board[(n - j - 1) * game->size + i] = tmp;
		}
	}
}

// Performs a move on the whole board.
// Returns whether any field was moved.
bool move_up(Game *game) {
	bool success = false;
	uint32_t x;

	// Move each column of the board seperately
	for (x = 0; x < game->size; x++) {
		success |= slide_array(game, game->board + x * game->size);
	}

	return success;
}

// The following move functions are all implemented by first
// rotating the board, performing a move up and rotating some more.
// They also return whether any tile was moved.

bool move_left(Game *game) {
	bool success;
	move_board(game);
	success = move_up(game);
	move_board(game);
	move_board(game);
	move_board(game);
	return success;
}

bool move_down(Game *game) {
	bool success;
	move_board(game);
	move_board(game);
	success = move_up(game);
	move_board(game);
	move_board(game);
	return success;
}

bool move_right(Game *game) {
	bool success;
	move_board(game);
	move_board(game);
	move_board(game);
	success = move_up(game);
	move_board(game);
	return success;
}

// Returns the number of empty fields in the board.
uint32_t count_empty(Game *game) {
	uint32_t x, y;
	uint32_t count = 0;
	for (x = 0; x < game->size; x++) {
		for (y = 0; y < game->size; y++) {
			if (get_value(game, x, y) == 0) {
				count++;
			}
		}
	}
	return count;
}

// Adds a 2 or 4 at a random empty position in the board.
void add_new_number(Game *game) {
	// Decide whether to add 2 or 4.
	int val = rand() % 2 == 0 ? 2 : 4;

	// Choose a random empty field.
	uint32_t empties = count_empty(game);

	if (empties == 0) {
		printf("ERROR: No more empty fields!");
		return;
	}

	int pos = rand() % empties;

	// We now want to find the board index of the pos-th empty element.

	int idx = 0;
	int passed_empties = 0;

	// To do this, we iterate through the board, keeping track of how many
	// empty values we've passed. Once we've passed enough, we stop and have our index.
	while (passed_empties <= pos) {
		if (game->board[idx] == 0) {
			passed_empties++;
		}
		idx++;
	}

	// Sanity check
	if (game->board[idx - 1] != 0) {
		printf("ERROR: Field not empty!!\n");
	}

	// Set the value
	game->board[idx - 1] = val;
}

// Returns whether there are any valid moves left to make.
bool has_valid_moves(Game *game) {
	bool any_moves_found = false;

	// Check for each of the 4 possible move directions.
	for (int dir = 0; dir < 4; dir++) {
		move_board(game);

		// Iterate over the whole board.
		for (uint32_t y = 0; y < game->size; y++) {
			uint32_t *row = game->board + y * game->size;
			for (uint32_t x = 0; x < game->size; x++) {
				// Check if it could be moved.
				if (find_target(row, x, 0) != x) {
					any_moves_found = true;
				}
			}
		}
	}

	return any_moves_found;
}

// Returns whether the game should end because it was won.
bool has_won(Game *game) {
	// Can't win in endless mode.
	if (game->mode == Endless) return false;

	// Check if any tile is greater than or equal to 2048.
	for (int i = 0; i < (game->size * game->size); i++) {
		if (game->board[i] >= 2048) return true;
	}

	// Otherwise, the game wasn't won (yet).
	return false;
}