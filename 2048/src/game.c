#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#include "common.h"

//#define SIZE 4

/*void drawBoard(uint32_t board[SIZE][SIZE]) {
	uint32_t x, y;
	for (y = 0; y < SIZE; y++) {
		for (x = 0; x < SIZE; x++) {
			printf("\n");
			for (x = 0; x < SIZE; x++) {
				if (board[x][y] != 0) {
					char s[8];
					sprintf(s, 8, "%u", (uint32_t)1 << board[x][y]);
					uint32_t t = 7 - strlen(s);
					printf("%*s%s%*s", t - t / 2, "", s, t / 2, "");
				}
				else {
					printf("   ·   ");
				}
			}
		}
	}
}*/

uint32_t find_target(uint32_t *array, uint32_t x, uint32_t stop) {
	uint32_t t;// if the position is already on the first, don't evaluate
	if (x == 0) {
		return x;
	}
	for (t = x - 1; t >= 0; t--) {
		if (array[t] != 0) {
			if (array[t] != array[x]) {// merge is not possible, take next position
				return t + 1;
			}
			return t;
		}
		else {	// we should not slide further, return this one
			if (t == stop) {
				return t;
			}
		}
	}// we did not find a
	return x;
}

bool slide_array(Game *game, uint32_t *array) {
	bool success = false;
	uint32_t x, t, stop = 0;

	for (x = 0; x < game->size; x++) {
		if (array[x] != 0) {
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

bool move_up(Game *game) {
	bool success = false;
	uint32_t x;
	for (x = 0; x < game->size; x++) {
		success |= slide_array(game, game->board + x * game->size);
	}

	return success;
}

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

bool find_pair_down(Game *game) {
	bool success = false;
	uint32_t x, y;
	for (x = 0; x < game->size; x++) {
		for (y = 0; y < game->size - 1; y++) {
			if (get_value(game, x, y) == get_value(game, x, y)) return true;
		}
	}
	return success;
}

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

void add_new_number(Game *game) {
	int val = rand() % 2 == 0 ? 2 : 4;
	
	uint32_t empties = count_empty(game);
	int pos = rand() % empties;

	int idx = 0;
	int passed_empties = 0;
	
	while (passed_empties <= pos) {
		if (game->board[idx] == 0) {
			passed_empties++;
		}
		idx++;
	}

	if (game->board[idx - 1] != 0) {
		printf("ERROR: Field not empty!!\n");
	}
	game->board[idx - 1] = val;
}

bool has_valid_moves(Game *game) {
	bool any_moves_found = false;

	for (int dir = 0; dir < 4; dir++) {
		move_board(game);

		for (int y = 0; y < game->size; y++) {
			uint32_t *row = game->board + y * game->size;
			for (int x = 0; x < game->size; x++) {
				if (find_target(row, x, 0) != x) {
					any_moves_found = true;
				}
			}
		}
	}

	return any_moves_found;
}