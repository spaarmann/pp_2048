#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "game.h"

// Error handling functions.

void sdl_error(Game *game, char *msg) {
	printf("SDL Error (%s): %s\n", msg, SDL_GetError());

	free_rendering_stuff(game);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	exit(1);
}

void img_error(Game *game, char *msg) {
	printf("IMG Error (%s): %s\n", msg, IMG_GetError());

	free_rendering_stuff(game);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	exit(1);
}

void ttf_error(Game *game, char *msg) {
	printf("TTF Error (%s): %s\n", msg, TTF_GetError());

	free_rendering_stuff(game);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	exit(1);
}

// Creates the window, renderer and loads some resources.
void create_renderer_and_window(Game *game, int width, int height) {
	game->window = NULL;
	game->renderer = NULL;
	game->tile_textures = NULL;
	game->startscreen_texture = NULL;
	game->endscreen_texture = NULL;

	// Tell SDL to intialize
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		sdl_error(game, "SDL_Init");
	}

	if (TTF_Init() != 0) {
		ttf_error(game, "TTF_Init");
	}

	int img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		img_error(game, "IMG_Init");
	}

	// Create a window.
	game->window = SDL_CreateWindow("2048",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, 0);

	if (game->window == NULL) {
		sdl_error(game, "CreateWindow");
	}

	// Create a renderer.
	game->renderer = SDL_CreateRenderer(game->window,
		-1, SDL_RENDERER_PRESENTVSYNC);

	if (game->renderer == NULL) {
		sdl_error(game, "CreateRenderer");
	}

	// Load the font used for the interface.
	game->interface_font = TTF_OpenFont("./res/font.ttf", 60);
	if (game->interface_font == NULL) {
		ttf_error(game, "OpenFont");
	}

	// Load the start screen texture.
	SDL_Surface *startscreen_surface = IMG_Load("./res/StartScreen.png");
	if (startscreen_surface == NULL) {
		img_error(game, "OpenImage");
	}

	game->startscreen_texture = SDL_CreateTextureFromSurface(game->renderer, startscreen_surface);
	SDL_FreeSurface(startscreen_surface);

	// Define positions and size of "Normal" and "Endless" buttons.
	SDL_Rect *rect = malloc(sizeof(SDL_Rect));
	rect->x = 80; rect->y = 130; rect->w = 470; rect->h = 130;
	game->start_normal_rect = rect;

	rect = malloc(sizeof(SDL_Rect));
	rect->x = 80; rect->y = 415; rect->w = 470; rect->h = 130;
	game->start_endless_rect = rect;

	// Load the end screen texture.
	SDL_Surface *endscreen_surface = IMG_Load("./res/EndScreen.png");
	if (endscreen_surface == NULL) {
		img_error(game, "OpenImage");
	}

	game->endscreen_texture = SDL_CreateTextureFromSurface(game->renderer, endscreen_surface);

	// Size and position "Play Again" button.
	rect = malloc(sizeof(SDL_Rect));
	rect->x = 80; rect->y = 290; rect->w = 470; rect->h = 130;
	game->play_again_rect = rect;
}

TileTexture *create_tile_texture(Game *game,
	TileTexture *last_texture,
	uint32_t value,
	SDL_Surface *base_tile_surface,
	TTF_Font *font,
	Uint32 color)
{
	TileTexture *tile_texture = malloc(sizeof(TileTexture));
	tile_texture->next = NULL;
	tile_texture->val = value;

	if (last_texture == NULL) {
		game->tile_textures = tile_texture;
	}
	else {
		last_texture->next = tile_texture;
	}


	Uint8 srcR, srcG, srcB, srcA;
	Uint8 resR, resG, resB, resA;

	SDL_PixelFormat *format = base_tile_surface->format;
	SDL_Surface *surface = SDL_CreateRGBSurface(0, base_tile_surface->w, base_tile_surface->h,
		format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask, format->Amask);

	Uint32 *srcPixels = (Uint32 *)base_tile_surface->pixels;
	Uint32 *resPixels = (Uint32 *)surface->pixels;

	for (int index = 0; index < (base_tile_surface->h * base_tile_surface->w); index++) {
		SDL_GetRGBA(srcPixels[index], format, &srcR, &srcG, &srcB, &srcA);

		if (srcA == 0xFF) {
			resR = (color >> 24) & 0x000000FF;
			resG = (color >> 16) & 0x000000FF;
			resB = (color >> 8) & 0x000000FF;
			resA = (color >> 0) & 0x000000FF;
		}
		else {
			resR = resG = resB = resA = 0x00;
		}

		resPixels[index] = SDL_MapRGBA(format, resR, resG, resB, resA);
	}

	if (value != 0) {
		char text[10];
		sprintf(text, "%d", value);

		SDL_Color textColor = { 255, 255, 255, 255 };
		SDL_Surface *fontSurface = TTF_RenderText_Blended(font, text, textColor);

		int font_width, font_height;
		TTF_SizeText(font, text, &font_width, &font_height);

		int tile_size = base_tile_surface->h;

		SDL_Rect dst = { tile_size / 2 - font_width / 2, tile_size / 2 - font_height / 2, font_width, font_height };
		SDL_BlitSurface(fontSurface, NULL, surface, &dst);

		SDL_FreeSurface(fontSurface);
	}

	tile_texture->texture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	return tile_texture;
}

void create_more_tile_textures(Game *game, TileTexture *last) {
	SDL_Surface *base_tile_surface = IMG_Load("./res/Tile.png");
	if (base_tile_surface == NULL) {
		img_error(game, "Load");
	}

	TTF_Font *font = TTF_OpenFont("./res/font.ttf", 200);
	if (font == NULL) {
		ttf_error(game, "OpenFont");
	}

	uint32_t current_step = 0;

	if (last != NULL) {
		current_step = (uint32_t)(log(last->val) / log(2));
	}

	uint32_t steps_total = current_step + 11; // 2 ^ 11 = 2048

	while (current_step <= steps_total) {
		uint32_t current_value = (uint32_t)pow(2, current_step);
		if (current_step == 0) current_value = 0;

		Uint8 r;
		if (current_step <= 11) {
			r = ((float)current_step / 11) * 0xF0 + 0x0F;
		}
		else {
			r = 0xFF;
		}
		Uint32 color = (r << 24) | 0x000000FF;

		last = create_tile_texture(
			game,
			last,
			current_value,
			base_tile_surface,
			font,
			color
		);

		current_step++;
	}

	SDL_FreeSurface(base_tile_surface);
	TTF_CloseFont(font);
}

void create_initial_tile_textures(Game *game) {
	create_more_tile_textures(game, NULL);
}

void free_rendering_stuff(Game *game) {
	// Free textures for the tiles.
	TileTexture *tex, *next;
	tex = game->tile_textures;
	while (tex != NULL) {
		SDL_DestroyTexture(tex->texture);
		next = tex->next;
		free(tex);
		tex = next;
	}

	// Free other textures.

	if (game->startscreen_texture != NULL) {
		SDL_DestroyTexture(game->startscreen_texture);
		free(game->start_normal_rect);
		free(game->start_endless_rect);
	}

	if (game->endscreen_texture != NULL) {
		SDL_DestroyTexture(game->endscreen_texture);
		free(game->play_again_rect);
	}

	// Close the font.
	if (game->interface_font != NULL)
		TTF_CloseFont(game->interface_font);

	if (game->renderer != NULL)
		SDL_DestroyRenderer(game->renderer);
	if (game->window != NULL)
		SDL_DestroyWindow(game->window);
}

// Displays a single square with the specified value at the specified point on the screen.
void display_square(Game *game, const uint32_t val, const SDL_Rect *rect) {
	TileTexture *tex = game->tile_textures;
	for (;;) {
		if (tex->val == val) {
			break;
		}
		if (tex->next == NULL) {
			create_more_tile_textures(game, tex);
		}

		tex = tex->next;
	}

	SDL_RenderCopy(game->renderer, tex->texture, NULL, rect);
}

const int UI_HEIGHT = 100;

// Displays the whole playing field.
void display_board(Game *game) {
	int display_width = game->window_width;
	int display_height = game->window_height - UI_HEIGHT;
	int square_width = display_width / game->size;
	int square_height = display_height / game->size;

	int square_size = square_width > square_height ? square_height : square_width;

	for (int x = 0; x < game->size; x++) {
		for (int y = 0; y < game->size; y++) {
			SDL_Rect rect;
			rect.x = x * square_size;
			rect.y = y * square_size + UI_HEIGHT;
			rect.w = square_size;
			rect.h = square_size;

			display_square(game, get_value(game, x, y), &rect);
		}
	}
}

// Displays the score text at the top.
void display_interface(Game *game) {
	char scoreText[40];
	sprintf(scoreText, "Score: %d", game->score);

	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface *surface = TTF_RenderText_Blended(game->interface_font, scoreText, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	int font_width, font_height;
	TTF_SizeText(game->interface_font, scoreText, &font_width, &font_height);

	SDL_Rect dst = { 25, 10, font_width, font_height };

	SDL_RenderCopy(game->renderer, texture, NULL, &dst);
}

void display_startscreen(Game *game) {
	SDL_RenderCopy(game->renderer, game->startscreen_texture, NULL, NULL);
}

void display_endscreen(Game *game) {
	SDL_RenderCopy(game->renderer, game->endscreen_texture, NULL, NULL);

	char scoreText[45];
	sprintf(scoreText, "Your Score: %d", game->score);

	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface *surface = TTF_RenderText_Blended(game->interface_font, scoreText, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	int font_width, font_height;
	TTF_SizeText(game->interface_font, scoreText, &font_width, &font_height);
	SDL_Rect dst = { 25, 80, font_width, font_height };

	SDL_RenderCopy(game->renderer, texture, NULL, &dst);
	SDL_DestroyTexture(texture);

	char *flavour_text;
	if (game->mode == Endless) {
		flavour_text = "Game Over";
	}
	else {
		if (has_won(game)) {
			flavour_text = "You won!";
		}
		else {
			flavour_text = "You lost!";
		}
	}

	surface = TTF_RenderText_Blended(game->interface_font, flavour_text, color);
	texture = SDL_CreateTextureFromSurface(game->renderer, surface);
	TTF_SizeText(game->interface_font, flavour_text, &font_width, &font_height);
	SDL_Rect dst2 = { 25, 20, font_width, font_height };
	
	SDL_RenderCopy(game->renderer, texture, NULL, &dst2);
	SDL_DestroyTexture(texture);
}