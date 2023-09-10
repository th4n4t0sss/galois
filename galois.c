#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

/* TODO: better way to define this variables */
int STEP = 100;
double LINE_THICKNESS = 5.0;
bool show_line = true;

int x_mouse = 0;
int y_mouse = 0;

int x_axes = 0;
int y_axes = 0;
/* TODO: text prompt for inserting mathematical function */
int f(int x) {
	return x * x;
}

void sdl_clean_up(SDL_Window *window, SDL_Renderer *renderer, TTF_Font *font) {
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	TTF_Quit();
	SDL_Quit();
}

/* rendering number text */
void render_number(SDL_Renderer *renderer, TTF_Font *font, int number, int x, int y) {
    SDL_Color textColor = {255, 255, 255};

    char numberText[16];
    snprintf(numberText, sizeof(numberText), "%d", number);

    SDL_Surface *surface = TTF_RenderText_Solid(font, numberText, textColor);
    if (!surface) {
        fprintf(stderr, "Text rendering failed: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "Texture creation failed: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void rendering_coordinate_system(SDL_Renderer *renderer, int width, int height, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	/*
	if y_mouse > height / 2) then
		y_axes = height / 2 + y_mouse
	else y_axes = height / 2 - y_mouse
	*/
	if (y_mouse > height / 2)
		y_axes = height / 2 - y_mouse;
	else y_axes = height / 2 + y_mouse;
	
    SDL_RenderDrawLine(renderer, 
						0, y_axes,
						width, y_axes); /* horizontal axes line */
	if (x_mouse > width / 2)
		x_axes = width / 2 - x_mouse;
	else x_axes = width / 2 + x_mouse;

	printf("%d\n", x_axes);
	/*
	 if mouse_x > widht / 2 then
		render width / 2 - mouse_x
	 else render width / 2 + mouse_x
	 */
    SDL_RenderDrawLine(renderer, 
						x_axes, 0, 
						x_axes, height); /* vertical axes line */

	int x_axes_number = -(width / 2 / STEP);
	int y_axes_number = height / 2 / STEP;

	for (int i=0; i<=width; i+=STEP) {
		SDL_RenderDrawLine(renderer,
				    i, height / 2 - 2,
				    i, height / 2 + 2);

		SDL_RenderDrawLine(renderer,
				   width / 2 - 2, i,
				   width / 2 + 2, i);

		render_number(renderer, font, x_axes_number, i, height / 2); /* rendering numbers on axes */
		render_number(renderer, font, y_axes_number, width / 2, i);

		x_axes_number++; 
		y_axes_number--;
	}
}

void ploting(SDL_Renderer *renderer, int width, int height, TTF_Font *font, double LINE_THICKNESS) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); /* color red for dots */

    int x_max = width / 2 / STEP; /* should be maximum coordinates for x but i am not sure that this is proper way */
    int y_max = height / 2 / STEP;
    double step = 1.0; /* TODO: get the step from STEP variable */

	int previous_x_pos = 0;
	int previous_y_pos = 0;

	for (double x = -x_max; x <= x_max; x += step) { /* looping through all the values of x */
        double function = f(x);
        int x_pos = x * STEP + width / 2;
        int y_pos = height / 2 - function * STEP;

		if (show_line) {
			/* TODO: draw line clever way */
			thickLineRGBA(renderer,
						  previous_x_pos, previous_y_pos,
						  x_pos, y_pos,
						  LINE_THICKNESS,
						  255,
						  0,
						  0,
						  255);

			previous_x_pos = x_pos;
			previous_y_pos = y_pos;
		} else 
			SDL_RenderDrawPoint(renderer, x_pos, y_pos);
    }
}

int main(int argc, char *argv[]) {
	int width,height;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError()) ;
		return EXIT_FAILURE ;
	}

	if (TTF_Init() != 0) {
		fprintf(stderr, "could not initialize TTF: %s\n", SDL_GetError()) ;
		return EXIT_FAILURE ;
	}
	
	SDL_Window *window = SDL_CreateWindow("Galois",
						SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
						SCREEN_WIDTH, SCREEN_HEIGHT,
						SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "could not initialize window: %s\n", SDL_GetError()) ;
		return EXIT_FAILURE;
	}
	
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
						     SDL_RENDERER_ACCELERATED |
						     SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		fprintf(stderr, "could not initialize renderer: %s\n", SDL_GetError()) ; 
		return EXIT_FAILURE;
	}

	TTF_Font *font = TTF_OpenFont("./fonts/Fixed-Sys.ttf", 14);
	
	SDL_Event event;
	SDL_StartTextInput();

	while (1) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					sdl_clean_up(window, renderer, font);
					return EXIT_SUCCESS;
				/* TODO: movement with mouse */
				case SDL_MOUSEBUTTONDOWN:
					SDL_GetMouseState(&x_mouse, &y_mouse);
					printf("%d : %d\n", x_mouse, y_mouse);
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_MINUS: /* decrease the coordinate number spectrume */
							STEP -= 5;
							if (LINE_THICKNESS > 2.0)
								LINE_THICKNESS -= 0.15;
							break;
						case SDLK_EQUALS: /* increase the coordinate number spectrume */
							STEP += 5;
							LINE_THICKNESS += 0.15;
							break;
						case SDLK_l: /* toggle line drawing on "l" */
							if (show_line) 
								show_line = false;
							else show_line = true;
							break;
						case SDLK_f: /* TODO: add text prompt for f(x) function */
							break;	
						default:
							break;
					}
					break;
				case SDL_WINDOWEVENT:
					/* TODO: call rendering_coordinates and ploting functions on window resize */
					/*if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
					    SDL_RenderClear(renderer);

						SDL_GetWindowSize(window, &width, &height);
						printf("%d : %d\n", width, height);

						rendering_coordinate_system(renderer, width, height, font);
						ploting(renderer, width , height, font, LINE_THICKNESS);
					}*/
					break;
				default:
					break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_GetWindowSize(window, &width, &height);

		rendering_coordinate_system(renderer, width, height, font);
		ploting(renderer, width , height, font, LINE_THICKNESS);
		SDL_RenderPresent(renderer);
	}

	sdl_clean_up(window, renderer, font);

	return EXIT_SUCCESS;
}
