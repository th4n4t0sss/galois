#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

#define PLOTTING_STEP 1.0

/* TODO: better way to define this variables */
int step = PLOTTING_STEP * 30;

double line_thickness = 3.0;
bool show_line = true;

int x_axes, y_axes;

/* TODO: text prompt for inserting mathematical function */
double f(double x) {
	return x;
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

	bool in_x = point_x - line_thickness <= x && x <= point_x + line_thickness;
	bool in_y = point_y - line_thickness <= y && y <= point_y + line_thickness;

	if (in_x && in_y)
		return true;
	return false;
}

/* rendering text TODO: render numbers also with this functions */
SDL_Texture* renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color textColor) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/* rendering number text */
void render_number(SDL_Renderer *renderer, TTF_Font *font, int number, int x, int y) { 
	SDL_Color textColor = {255, 255, 255, 255};

    char numberText[8];
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

void plotting(SDL_Renderer *renderer, int width, double line_thickness, TTF_Font *font) {
	int x_max = width / step;
	if (x_axes > 0)
		x_max += x_axes / step;
	else x_max -= x_axes / step;

	int previous_x_pos = 0;
	int previous_y_pos = 0;

	bool draw_line = false;
	for (double x = -x_max; x <= x_max; x += PLOTTING_STEP) { /* looping through all values of x */
        double function = f(x);
		
        int x_pos = x * step + x_axes;
        int y_pos = y_axes - function * step;

		if (show_line) {
			if (draw_line) {
				thickLineRGBA(renderer,
								previous_x_pos, previous_y_pos,
								x_pos, y_pos,
								line_thickness,
								255, 0, 0, 255);
			}

			previous_x_pos = x_pos;
			previous_y_pos = y_pos;
			draw_line = true;
		} else {
			/* drawing thick dot but because SDL_DrawPoint doesn't have point thickness argument */
			thickLineRGBA(renderer,
						  x_pos, y_pos,
						  x_pos,y_pos,
						  line_thickness,
						  255, 0, 0, 255);
		}

		if (check_mouse_hover(x_pos, y_pos)) {
			render_number(renderer, font, x, x_pos + line_thickness + 10, y_pos);
			render_number(renderer, font, (int)function, x_pos + line_thickness + 30, y_pos);
		}
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

	int x_max = width / step;
	if (x_axes > width / 2)
		x_max += x_axes / step;
	else x_max -= x_axes / step;

	int y_max = height / step;
	if (y_axes > height / 2)
		y_max += y_axes / step;
	else y_max -= y_axes / step;

	int x_axes_number = -(x_max);
	int y_axes_number = y_max;

	for (double i = -x_max; i <= x_max; i += PLOTTING_STEP) { /* looping through all values of x */
        int x_pos = x_axes + i * step;
		/* TODO: fix the y axes number rendering bug */
        int y_pos = y_axes - i * step;
		
		SDL_RenderDrawLine(renderer, /* horizontal */
					x_pos, y_axes - 2,
					x_pos, y_axes + 2);

		SDL_RenderDrawLine(renderer, /* veritcal */
					x_axes - 2, y_pos,
					x_axes + 2, y_pos);

		/* rendering numbers on axes */
		render_number(renderer, font, x_axes_number, x_pos, y_axes); /* horizontal */
		render_number(renderer, font, y_axes_number, x_axes, y_pos); /* vertical */ 

		x_axes_number++;
		y_axes_number--;
	}
}

int main(int argc, char *argv[]) {
	int width, height;
	int input_width, input_height;

	char inputText[256] = "f(x) = ";

	bool running = true;
	bool show_prompt = false;

    SDL_Color textColor = { 255, 255, 255, 255 };

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

	TTF_Font *font = TTF_OpenFont("./fonts/terminalvector.ttf", 12);
	if (font == NULL) {
		sdl_clean_up(window, renderer, font);
		fprintf(stderr, "font not found: %s\n", SDL_GetError()) ;
		return EXIT_FAILURE;
	}

	SDL_GetWindowSize(window, &width, &height);

	x_axes = width / 2;
	y_axes = height / 2;

	SDL_Event event;
	SDL_StartTextInput();

	while (running) {
		while(SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_MOUSEMOTION:
					if (event.motion.state & SDL_BUTTON_LMASK) {
						x_axes += event.motion.xrel;
						y_axes += event.motion.yrel;
					}
					break;
				case SDL_KEYDOWN:
					/* TODO: rewrite this bullshit idiot */
					if (show_prompt) {
						if (strlen(inputText) < 255) {
							if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
								inputText[strlen(inputText) - 1] = '\0';
							} else if (event.key.keysym.sym == SDLK_ESCAPE) {
								show_prompt = false;
							} else {
								strcat(inputText, SDL_GetKeyName(event.key.keysym.sym));
							}
						}
					} else {
						switch(event.key.keysym.sym) {
							case SDLK_MINUS: /* decrease the coordinate number spectrume */
								if (step > 1.0)
									step -= 5;
								if (line_thickness > 2.0)
									line_thickness -= 0.15;
								break;
							case SDLK_EQUALS: /* increase the coordinate number spectrume */
								step += 5;
								if (line_thickness < 6.0)
									line_thickness += 0.15;
								break;
							case SDLK_0:
								x_axes = width / 2;
								y_axes = height / 2;
								break;
							case SDLK_l: /* toggle line drawing on "l" */
								show_line = !show_line;
								break;
							case SDLK_f: /* TODO: add text prompt for f(x) function */
								show_prompt = true;
								break;
							default:
								break;
						}
					}
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
						x_axes = width / 2;
						y_axes = height / 2;
					}
					break;
				default:
					break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_GetWindowSize(window, &width, &height);

		input_width = width / 2;
		input_height = height / 15;

		if (show_prompt) {
			/* copy pasta code need to be rewritten normally or i AI will replace me */
			// Render the text input box
			SDL_Rect inputBoxRect = { (width - input_width) / 2, (height - input_height) / 2, input_width, input_height };
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(renderer, &inputBoxRect);

			SDL_Texture *textTexture = renderText(renderer, font, inputText, textColor);
			SDL_Rect textRect = { inputBoxRect.x + 10, inputBoxRect.y + (inputBoxRect.h - 24) / 2, 0, 0 };
			SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
			SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
			SDL_DestroyTexture(textTexture);
		} else {
			rendering_coordinates(renderer, width, height, font);
			plotting(renderer, width, line_thickness, font);
		}

		SDL_RenderPresent(renderer);
	}

	sdl_clean_up(window, renderer, font);

	return EXIT_SUCCESS;
}
