#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int STEP = 100;

double f(double x) {
	return x * x;
}

/* rendering number text */
void render_number(SDL_Renderer *renderer, TTF_Font *font, int number, int x, int y) {
    SDL_Color textColor = {255, 255, 255};

    char numberText[5];
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

void ploting(SDL_Renderer *renderer, int width, int height, TTF_Font *font) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderDrawLine(renderer, 
						0, height / 2, 
						width, height / 2); /* Drawing horizontal axes line */

    SDL_RenderDrawLine(renderer, 
						width / 2, 0, 
						width / 2, height); /* Drawing vertical axes line */

	int positive_x = 0;
	int negative_x = -(width / 2 / STEP); /* valuing start amount of negative_x variable */

	/* Rendering numbers on axes */
	for (int i=0; i<=width; i+=STEP) {
		SDL_RenderDrawLine(renderer,
				    i, height / 2 - 2,
				    i, height / 2 + 2);

		SDL_RenderDrawLine(renderer,
				   width / 2 - 2, i,
				   width / 2 + 2, i);
		render_number(renderer, font, positive_x, width / 2 + i, height / 2);
		positive_x++;
		
		/*if (negative_x != 0)
			negative_x++; TODO: iplement negative number format for x coordinates */
	}

    int x_max = width / 2 / STEP; /* should be maximum coordinates for x but i am not sure that this is proper way */
    int y_max = height / 2 / STEP; /* same thing but for y */
    double step = 1.00; /* TODO: get the step from STEP variable */

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); /* color red for dots */
	for (double x = -x_max; x <= x_max; x += step) { /* looping through all the values of x */
        double function = f(x);
        int x_pos = (int)(x * STEP) + width / 2;
        int y_pos = height / 2 - (int)(function * STEP);

        SDL_RenderDrawPoint(renderer, x_pos, y_pos);
    }
	// TODO: draw line between points with thicklineRGBA()
}

int main(int argc, char *argv[]) {

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

	TTF_Font *font = TTF_OpenFont("./Fixed-Sys.ttf", 15);
	
	SDL_Event event;
	int quit = 0;
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_PERIOD: /* increase the coordinate number spectrume */
							STEP -= 10;
							break;
						case SDLK_COMMA: /* reduce the coordinate number spectrume */
							STEP += 10;
							break;
						default:
							break;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				default: {}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		int width = SDL_GetWindowSurface(window)->w;
		int height = SDL_GetWindowSurface(window)->h;

		// printf("width: %d\nheight: %d\n", width, height);
		ploting(renderer, width, height, font);

		SDL_RenderPresent(renderer);
	}

	TTF_CloseFont(font);
	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
