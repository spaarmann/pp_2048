#include "common.h"

uint32_t get_value(const Game *game, int x, int y) {
	return game->board[y + x * game->size];
}