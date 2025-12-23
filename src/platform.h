#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
} platform_t;

void init_platform(platform_t *platform, char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
void destroy_platform(platform_t *platform);
void update_platform(platform_t *platform, void const *buffer, int pitch);
bool process_input(uint8_t *keys);

#endif // !PLATFORM_H
