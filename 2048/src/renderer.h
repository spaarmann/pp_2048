#pragma once
#include <stdio.h>
#include <SDL.h>

#include "common.h"

void sdl_error(char *msg);

// TODO: Proper resource cleanup at exit
SDL_Renderer *create_renderer_and_window(int width, int height);

void display_board(Game *game);