#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL2_gfxPrimitives.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

/* TODO: better way to define this variables */
int step = 50;
double plotting_step = 0.1; /* TODO: get the step from step variable */

double line_thicnkess = 3.0;
bool show_line = true;

int x_mouse = 0;
int y_mouse = 0;

int x_axes, y_axes;

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

int check_mouse_hover(int point_x, int point_y) {
	int x,y;
	SDL_GetMouseState(&x, &y);

	if (x == point_x && y == point_y)
		return true;
	return false;
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

void plotting(SDL_Renderer *renderer, int width, double line_thicnkess, TTF_Font *font) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); /* color red for dots */

	int x_max = width / step; /* should be maximum coordinates for x but i am not sure that this is proper way */

	int previous_x_pos = 0;
	int previous_y_pos = 0;

	bool draw_line = false;
	for (double x = -x_max; x <= x_max; x += plotting_step) { /* looping through all values of x */
        double function = f(x);
		
        int x_pos = x * step + x_axes;
        int y_pos = y_axes - function * step;

		if (show_line) {
			/* TODO: draw line clever way */
			if (draw_line) {
				/*thickLineRGBA(renderer,
								previous_x_pos, previous_y_pos,
								x_pos, y_pos,
								line_thicnkess,
								255, 0, 0, 255);*/
				SDL_RenderDrawLine(renderer,
									previous_x_pos, previous_y_pos,
									x_pos, y_pos);
			}

			previous_x_pos = x_pos;
			previous_y_pos = y_pos;
			draw_line = true;
		} else {
			/* drawing thick dot but because SDL_DrawPoint doesn't have point thickness argument */
			/* thickLineRGBA(renderer,
						  x_pos, y_pos,
						  x_pos,y_pos,
						  line_thicnkess,
						  255, 0, 0, 255);*/
			SDL_RenderDrawPoint(renderer, x_pos, y_pos);

		}

		/*if (check_mouse_hover(x_pos, y_pos)) {
			render_number(renderer, font, x, x_pos + line_thicnkess + 10, y_pos);
			render_number(renderer, font, (int)function, x_pos + line_thicnkess + 20, y_pos);
		}*/
    }
}

void rendering_coordinates(SDL_Renderer *renderer, int width, int height, TTF_Font *font) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_RenderDrawLine(renderer,
						0, y_axes,
						width, y_axes); /* horizontal axes line */

	SDL_RenderDrawLine(renderer,
						x_axes, 0,
						x_axes, height); /* vertical axes line */

	int x_axes_number = -(width / step);
	int y_axes_number = height / step;

	// ???
	for (int i=-width; i<=width; i+=step) {
		SDL_RenderDrawLine(renderer,
				    x_axes + i, y_axes - 2,
				    x_axes + i, y_axes + 2);

		SDL_RenderDrawLine(renderer,
				   x_axes - 2, y_axes + i,
				   x_axes + 2, y_axes + i);

		/* rendering numbers on axes */
		render_number(renderer, font, x_axes_number, x_axes + i, y_axes); /* horizontal */
		render_number(renderer, font, y_axes_number, x_axes, y_axes + i); /* vertical */ 

		x_axes_number++;
		y_axes_number--;
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

	TTF_Font *font = TTF_OpenFont("./fonts/terminalvector.ttf", 10);
	if (font == NULL) {
		sdl_clean_up(window, renderer, font);
		fprintf(stderr, "font not found: %s\n", SDL_GetError()) ;
		return EXIT_FAILURE;
	}

	SDL_Event event;
	SDL_StartTextInput();

	while (1) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					sdl_clean_up(window, renderer, font);
					return EXIT_SUCCESS;
				case SDL_MOUSEMOTION:
					if (event.motion.state & SDL_BUTTON_LMASK) {
						x_axes += event.motion.xrel;
						y_axes += event.motion.yrel;
					}
					/* if mouse x and y is equal to point coordinates
					 give small box with coordinates */
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_MINUS: /* decrease the coordinate number spectrume */
							if (step > 1.0)
								step -= 5;
							if (line_thicnkess > 2.0)
								line_thicnkess -= 0.15;
							break;
						case SDLK_EQUALS: /* increase the coordinate number spectrume */
							step += 5;
							if (line_thicnkess < 6.0)
								line_thicnkess += 0.15;
							break;
						case SDLK_0:
							x_axes = width / 2;
							y_axes = height / 2;
							break;
						case SDLK_l: /* toggle line drawing on "l" */
							show_line = !show_line;
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
		plotting(renderer, width, line_thicnkess, font);

		SDL_RenderPresent(renderer);
	}

	sdl_clean_up(window, renderer, font);

	return EXIT_SUCCESS;
}
