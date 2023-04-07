#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils.h"

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef ssize_t isize;

#define width 500
#define height 500
#define PI 3.14159
const i32 length = width * 2;

typedef struct {
	f32 x1, y1, x2, y2;
} Vector;

typedef struct {
	f32 x, y;
} Point_f;

typedef struct {
	i32 x, y;
} Point_i;

Vector new_ray(Point_i center_point, i32 angle) {

	f32 x2 = center_point.x + length * cos(angle * PI / 180);
	f32 y2 = center_point.y + length * sin(angle * PI / 180);

	return (Vector){center_point.x, center_point.y, x2, y2};
}

f32 distance_between(Point_i p1, Point_f p2) { return sqrt(powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2)); }

void draw_vector(SDL_Renderer *renderer, Vector vector, SDL_Color color) {
	// Calculate the vector_width and vector_height of the rectangle based on
	// the vector length and angle
	f32 vector_width = sqrt(pow(vector.x2 - vector.x1, 2) + pow(vector.y2 - vector.y1, 2));
	f32 vector_height = 2.5f; // You can set this to any desired value

	// Calculate the angle of the vector in radians
	f32 angle = atan2(vector.y2 - vector.y1, vector.x2 - vector.x1);

	// Set the render color
	// SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	// Create a texture for the rectangle
	SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(0, vector_width, vector_height, 32, SDL_PIXELFORMAT_RGBA8888);
	SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, color.r, color.g, color.b, color.a));
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	// Set the render target and copy the texture to it with rotation
	SDL_Rect dstRect = {vector.x1, vector.y1 - vector_height / 2, vector_width, vector_height};
	SDL_Point center = {0, vector_height / 2};
	SDL_RenderCopyEx(renderer, tex, NULL, &dstRect, angle * 180 / M_PI, &center, SDL_FLIP_NONE);
	SDL_DestroyTexture(tex);
}

static inline Point_f is_intersecting(Vector vector1, Vector vector2) {

	const f32 d = ((vector1.x1 - vector1.x2) * (vector2.y1 - vector2.y2)) - ((vector1.y1 - vector1.y2) * (vector2.x1 - vector2.x2));

	if (fabsf(d) < 0.000001f) {
		return (Point_f){NAN, NAN};
	}

	const f32 t = (((vector1.x1 - vector2.x1) * (vector2.y1 - vector2.y2)) - ((vector1.y1 - vector2.y1) * (vector2.x1 - vector2.x2))) / d;

	const f32 u = (((vector1.x1 - vector2.x1) * (vector1.y1 - vector1.y2)) - ((vector1.y1 - vector2.y1) * (vector1.x1 - vector1.x2))) / d;

	return (t >= 0 && t <= 1 && u >= 0 && u <= 1) ? ((Point_f){vector1.x1 + (t * (vector1.x2 - vector1.x1)), vector1.y1 + (t * (vector1.y2 - vector1.y1))})

												  : ((Point_f){NAN, NAN});
}

int main(int argc, char *argv[]) {
	// Initialize SDL and create a window and renderer
	SDL_Window *window = init("raycast", width, height);
	if (window == NULL) {
		printf("Failed to initialize SDL_window!\n");
		return -1;
	}
	SDL_Renderer *renderer = SDL_GetRenderer(window);

	if (!init_mixer()) {
		printf("Failed to initialize mixer!\n");
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	Point_i mouse;

	Uint32 mouseState;

	isize boundries_length = 3;
	Vector boundries[boundries_length];

	boundries[0] = (Vector){300, 100, 300, 300};
	boundries[1] = (Vector){50, 100, 100, 400};
	boundries[2] = (Vector){50, 300, 300, 300};

	SDL_Color green = {0, 255, 0, 255};
	SDL_Color red = {255, 0, 0, 255};
	SDL_Color black = {0, 0, 0, 255};
	SDL_Color white = {255, 255, 255, 255};
	SDL_Color gray = {201, 200, 200, 255};

	SDL_Event e;

	u16 ray_num = 360;
	u16 ray_num_initial = 0;

	bool quit = false;
	while (!quit) {
		// Handle events
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				// Change the direction of the square with arrow keys
				switch (e.key.keysym.sym) {
				case SDLK_DOWN:
					ray_num -= 10;
					break;
				case SDLK_UP:
					ray_num += 10;
					break;
				case SDLK_LEFT:
					ray_num_initial -= 10;
					break;
				case SDLK_RIGHT:
					ray_num_initial += 10;
					break;
				}
			}
		}

		// Clear the screen
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		mouseState = SDL_GetMouseState(&mouse.x, &mouse.y);

		for (int i = ray_num_initial; i < ray_num; i++) {
			Point_f *closest = NULL;
			Vector ray = new_ray(mouse, i);

			f32 record = 1000000000.0;
			for (int j = 0; j < boundries_length; j++) {
				draw_vector(renderer, boundries[j], red);
				Point_f intersection = is_intersecting(ray, boundries[j]);

				if (!isnan(intersection.x)) {
					f32 distance = distance_between(mouse, intersection);
					if (distance < record) {
						record = distance;
						Point_f closest_point = {intersection.x, intersection.y};
						closest = &closest_point;
					}
				}
			}

			if (closest != NULL) {
				ray.x2 = closest->x;
				ray.y2 = closest->y;
				draw_vector(renderer, ray, white);
			} else {
				draw_vector(renderer, ray, gray);
			}
		}

		// for (int i = 0; i < boundries_length; i++) {
		// 	draw_vector(renderer, boundries[i], red);
		// }

		// Present the renderer to the screen
		SDL_RenderPresent(renderer);
	}

	// Clean up resources and exit
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
