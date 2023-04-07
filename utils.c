#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <stdbool.h>

#define MAX_SOUNDS 100
#define MAX_MUSIC 100

Mix_Chunk *sounds[MAX_SOUNDS];
Mix_Music *music[MAX_MUSIC];
int num_sounds = 0;
int num_music = 0;

int rand_int(int min, int max) {
	srand(time(NULL));
	return rand() % (max - min + 1) + min;
}

int load_music(const char *filename) {
	Mix_Music *m = NULL;
	m = Mix_LoadMUS(filename);
	if (m == NULL) {
		printf("Failed to load music. SDL_Mixer error: %s\n", Mix_GetError());
		return -1;
	}
	music[num_music++] = m;
	return num_music - 1;
}

int load_sound(const char *filename) {
	Mix_Chunk *m = NULL;
	m = Mix_LoadWAV(filename);
	if (m == NULL) {
		printf("Failed to load music. SDL_Mixer error: %s\n", Mix_GetError());
		return -1;
	}
	sounds[num_sounds++] = m;
	return num_sounds - 1;
}

int volume_sound;
void set_volume_sound(int v) { volume_sound = (MIX_MAX_VOLUME * v) / 100; }

int volume_music;
void set_volume_music(int v) { volume_music = (MIX_MAX_VOLUME * v) / 100; }

void play_music(int m) {
	if (Mix_PlayingMusic() == 0) {
		Mix_VolumeMusic(volume_music);
		Mix_PlayMusic(music[m], -1);
	}
}

void play_sound(int s) {
	Mix_Volume(-1, volume_sound);
	Mix_PlayChannel(-1, sounds[s], 0);
}

bool init_mixer() {
	Mix_Init(MIX_INIT_MP3);
	SDL_Init(SDL_INIT_AUDIO);
	// if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
	if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_Mixer couldnt init. Err: %s\n", Mix_GetError());
		return false;
	}
	set_volume_music(80);
	set_volume_sound(80);
	return true;
}

void quit_mixer() {
	for (int s = 0; s < num_sounds; s++) {
		Mix_FreeChunk(sounds[s]);
		sounds[s] = NULL;
	}
	for (int s = 0; s < num_music; s++) {
		Mix_FreeMusic(music[s]);
		music[s] = NULL;
	}
	Mix_Quit();
}

void toggle_play() {
	if (Mix_PausedMusic() == 1) {
		Mix_ResumeMusic();
	} else {
		Mix_PauseMusic();
	}
}

SDL_Window *init(char *title, int width, int height) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return NULL;
	}

	// Initialize TTF
	if (TTF_Init() < 0) {
		printf("TTF could not initialize! TTF Error: %s\n", TTF_GetError());
		return NULL;
	}

	// Create a window
	SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return NULL;
	}

	// Create a renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return NULL;
	}

	// Set the renderer's draw color to white
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

	// Clear the screen
	SDL_RenderClear(renderer);

	// Present the renderer to the screen
	SDL_RenderPresent(renderer);

	return window;
}

// draw_text function with antialiasing support
bool draw_text(SDL_Renderer *renderer,int font_size, int x, int y, const char *text) {
	// Initialize SDL_ttf library
	if (TTF_Init() == -1) {
		printf("Failed to initialize SDL_ttf: %s\n", TTF_GetError());
		return false;
	}

	// Load font file
	TTF_Font *font = TTF_OpenFont("/home/dennis/.local/share/fonts/TTF/Roboto/Roboto-Regular.ttf", font_size);
	if (font == NULL) {
		printf("Failed to load font: %s\n", TTF_GetError());
		return false;
	}

	// Create SDL_Surface for text with antialiasing
	SDL_Color color = {255, 255, 255, 255}; // white color
	SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
	if (surface == NULL) {
		printf("Failed to render text: %s\n", TTF_GetError());
		TTF_CloseFont(font);
		return false;
	}

	// Create SDL_Texture from surface
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		printf("Failed to create texture: %s\n", SDL_GetError());
		SDL_FreeSurface(surface);
		TTF_CloseFont(font);
		return false;
	}

	// Set texture alpha blend mode to support transparency
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	// Set render draw color to white for transparency
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	// Render texture to screen
	SDL_Rect dstrect = {x, y, surface->w, surface->h};
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	// Free resources
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);
	TTF_Quit();
	return true;
}
