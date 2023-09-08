#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define STEP 50

int f(int x) {
	return pow(x, 2);
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

		/* Ploting f(x) function */
		int y = f(positive_x);
		printf("%d\n", y);
	}
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

	TTF_Font *font = TTF_OpenFont("./Quarto-Regular.ttf", 16);
	
	SDL_Event event;
	int quit = 0;
	while (!quit) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
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

		printf("width: %d\nheight: %d\n", width, height);
		ploting(renderer, width, height, font);

		SDL_RenderPresent(renderer);
	}

	TTF_CloseFont(font);
	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}
