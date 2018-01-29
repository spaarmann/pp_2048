#pragma once

#include <stdbool.h>

#include "common.h"

bool do_move_up(Game *game);
bool do_move_down(Game *game);
bool do_move_right(Game *game);
bool do_move_left(Game *game);

void add_new_number(Game *game);
bool has_valid_moves(Game *game);

bool has_won(Game *game);