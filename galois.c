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
double step = 0.1; /* TODO: get the step from STEP variable */

double LINE_THICKNESS = 4.0;
bool SHOW_LINE = true;

int x_mouse = 0;
int y_mouse = 0;

int x_axes, y_axes;

int previous_x_pos = 0;
int previous_y_pos = 0;

/* TODO: text prompt for inserting mathematical function */
double f(double x) {
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
    SDL_Color textColor = {255, 255, 255, 255};

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

void rendering_coordinates(SDL_Renderer *renderer, int width, int height, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderDrawLine(renderer,
						0, y_axes,
						width, y_axes); /* horizontal axes line */

    SDL_RenderDrawLine(renderer,
						x_axes, 0,
						x_axes, height); /* vertical axes line */

	int x_axes_number = -(x_axes / STEP);
	int y_axes_number = y_axes / STEP;

	for (int i=0; i<=width; i+=STEP) {
		SDL_RenderDrawLine(renderer,
				    i, y_axes - 2,
				    i, y_axes + 2);

		SDL_RenderDrawLine(renderer,
				   x_axes - 2, i,
				   x_axes + 2, i);

		/* rendering numbers on axes */
		render_number(renderer, font, x_axes_number, i, y_axes);
		render_number(renderer, font, y_axes_number, x_axes, i);

		x_axes_number++;
		y_axes_number--;
	}
}

void plotting(SDL_Renderer *renderer, int width, double LINE_THICKNESS) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); /* color red for dots */

    int x_max = width / STEP; /* should be maximum coordinates for x but i am not sure that this is proper way */

	for (double x = -x_max; x <= x_max; x += step) { /* looping through all the values of x */
        double function = f(x);
		
        int x_pos = x * STEP + x_axes;
        int y_pos = y_axes - function * STEP;

		if (SHOW_LINE) {
			/* TODO: draw line clever way */
			thickLineRGBA(renderer,
						  previous_x_pos, previous_y_pos,
						  x_pos, y_pos,
						  LINE_THICKNESS,
						  255, 0, 0, 255);

			previous_x_pos = x_pos;
			previous_y_pos = y_pos;
		} else {
			/* drawing thick dot but because SDL_DrawPoint doesn't have point thickness argument */
			thickLineRGBA(renderer,
						  x_pos, y_pos,
						  x_pos,y_pos,
						  LINE_THICKNESS,
						  255, 0, 0, 255);
		}
    }
}

int main(int argc, char *argv[]) {
	int width, height;

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

	SDL_GetWindowSize(window, &width, &height);

	x_axes = width / 2;
	y_axes = height / 2;

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

	TTF_Font *font = TTF_OpenFont("./fonts/Fixed-Sys.ttf", 17);

	SDL_Event event;
	SDL_StartTextInput();

	while (1) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					sdl_clean_up(window, renderer, font);
					return EXIT_SUCCESS;
				case SDL_MOUSEMOTION:
					/* TODO: find better way to declare this */
					if (event.button.button == SDL_BUTTON_LEFT) {
						x_axes += event.motion.xrel;
						y_axes += event.motion.yrel;
					}
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_MINUS: /* decrease the coordinate number spectrume */
							if (STEP > 1.0)
								STEP -= 5;
							if (LINE_THICKNESS > 2.0)
								LINE_THICKNESS -= 0.15;
							break;
						case SDLK_EQUALS: /* increase the coordinate number spectrume */
							STEP += 5;
							LINE_THICKNESS += 0.15;
							break;
						case SDLK_l: /* toggle line drawing on "l" */
							if (SHOW_LINE)
								SHOW_LINE = false;
							else SHOW_LINE = true;
							break;
						case SDLK_f: /* TODO: add text prompt for f(x) function */
							break;
						default:
							break;
					}
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						x_axes = width / 2;
						y_axes = height / 2;
					}
					/* TODO: call rendering_coordinates and ploting functions on window resize */
					/*if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
					    SDL_RenderClear(renderer);

						SDL_GetWindowSize(window, &width, &height);
						printf("%d : %d\n", width, height);

						rendering_coordinate_system(renderer, width, height, font);
						plotting(renderer, width , height, font, LINE_THICKNESS);
					}*/
					break;
				default:
					break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_GetWindowSize(window, &width, &height);

		rendering_coordinates(renderer, width, height, font);
		plotting(renderer, width, LINE_THICKNESS);

		SDL_RenderPresent(renderer);
	}

	sdl_clean_up(window, renderer, font);

	return EXIT_SUCCESS;
}
