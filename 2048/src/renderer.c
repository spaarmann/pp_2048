#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

void sdl_error(char *msg) {
	printf("SDL Error (%s): %s\n", msg, SDL_GetError());
	SDL_Quit();
	// TODO: Resource cleanup
	exit(1);
}

void create_renderer_and_window(SDL_Window **window, SDL_Renderer **renderer, int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		sdl_error("SDL_Init");
	}

	if (TTF_Init() != 0) {
		// TODO: TTF_Error
		sdl_error("TTF_Init");
	}

	int img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		// TODO: Img_Error
		sdl_error("IMG_Init");
	}

	*window = SDL_CreateWindow("2048",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, 0);

	if (*window == NULL) {
		sdl_error("CreateWindow");
	}

	*renderer = SDL_CreateRenderer(*window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	if (*renderer == NULL) {
		sdl_error("CreateRenderer");
	}
}

void create_initial_tile_textures(Game *game) {
	SDL_Surface *base_tile_surface = IMG_Load("./res/Tile.png");
	if (base_tile_surface == NULL) {
		// TODO: IMG_Error
	}

	TTF_Font *font = TTF_OpenFont("./res/font.ttf", 256);

	uint32_t current_step = 1;
	uint32_t steps_total = 11; // 2 ^ 11 = 2048

	TileTexture *current_texture = NULL;

	SDL_Surface *current_surface;

	while (current_step <= steps_total) {
		uint32_t current_value = (uint32_t) pow(2, current_step);
		if (current_texture == NULL) {
			current_texture = malloc(sizeof(TileTexture));
			game->tile_textures = current_texture;
		}
		else {
			current_texture->next = malloc(sizeof(TileTexture));
			current_texture = current_texture->next;
		}

		current_texture->val = current_value;

		Uint8 srcR, srcG, srcB, srcA;
		Uint8 resR, resG, resB, resA;

		SDL_PixelFormat *format = base_tile_surface->format;
		current_surface = SDL_CreateRGBSurface(0, base_tile_surface->w, base_tile_surface->h,
			format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask, format->Amask);

		Uint32 *srcPixels = (Uint32 *)base_tile_surface->pixels;
		Uint32 *resPixels = (Uint32 *)current_surface->pixels;

		for (int index = 0; index < (base_tile_surface->h * base_tile_surface->w); index++) {
			SDL_GetRGBA(srcPixels[index], format, &srcR, &srcG, &srcB, &srcA);

			if (srcA == 0xFF) {
				resR = ((float) current_step / steps_total) * 0xF0 + 0x0F;
				resG = 0x00;
				resB = 0x00;
				resA = 0xFF;
			}
			else {
				resR = resG = resB = 0x00;
				resA = 0x00;
			}

			resPixels[index] = SDL_MapRGBA(format, resR, resG, resB, resA);
		}

		char text[10];
		sprintf(text, "%d", current_value);

		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface *fontSurface = TTF_RenderText_Blended(font, text, textColor);

		int font_width, font_height;
		TTF_SizeText(font, text, &font_width, &font_height);

		int tile_size = base_tile_surface->h;

		SDL_Rect dst = { tile_size / 2 - font_width / 2, tile_size / 2 - font_height / 2, font_width, font_height};
		SDL_BlitSurface(fontSurface, NULL, current_surface, &dst);

		current_texture->texture = SDL_CreateTextureFromSurface(game->renderer, current_surface);
		SDL_FreeSurface(current_surface);
		SDL_FreeSurface(fontSurface);

		current_step++;
	}

	TTF_CloseFont(font);
	SDL_FreeSurface(base_tile_surface);
}

void display_square(Game *game, const uint32_t val, const SDL_Rect *rect) {
	TileTexture *tex = game->tile_textures;
	for (;;) {
		if (tex->val == val) {
			break;
		}
		tex = tex->next;
	}

	SDL_RenderCopy(game->renderer, tex->texture, NULL, rect);
}

void display_board(Game *game) {
	int square_width = game->window_width / game->size;
	int square_height = game->window_height / game->size;

	int square_size = square_width > square_height ? square_height : square_width;

	for (int x = 0; x < game->size; x++) {
		for (int y = 0; y < game->size; y++) {
			SDL_Rect rect;
			rect.x = x * square_size;
			rect.y = y * square_size;
			rect.w = square_size;
			rect.h = square_size;

			display_square(game, get_value(game, x, y), &rect);
		}
	}
}

