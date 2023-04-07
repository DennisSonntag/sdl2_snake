#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <stdbool.h>

int rand_int(int min, int max);

int load_music(const char *filename);

int load_sound(const char *filename);

void set_volume_music(int volume);

void set_volume_sound(int volume);

void play_music(int music);

void play_sound(int sound);

bool init_mixer();

void quit_mixer();

void toggle_play();

SDL_Window *init(char *title, int width, int height);

bool draw_text(SDL_Renderer *renderer, int font_size, int x, int y, const char *text);
