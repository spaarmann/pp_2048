#pragma once

#include <stdbool.h>

#include "common.h"

bool move_up(Game *game);
bool move_down(Game *game);
bool move_right(Game *game);
bool move_left(Game *game);

void add_new_number(Game *game);
bool has_valid_moves(Game *game);

bool has_won(Game *game);