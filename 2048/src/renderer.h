#pragma once
#include <stdio.h>
#include <SDL.h>

#include "common.h"

void sdl_error(char *msg);

// TODO: Proper resource cleanup at exit
void create_renderer_and_window(Game *game, int width, int height);
void create_initial_tile_textures(Game *game);

void display_board(Game *game);
void display_interface(Game *game);

void display_endscreen(Game *game);